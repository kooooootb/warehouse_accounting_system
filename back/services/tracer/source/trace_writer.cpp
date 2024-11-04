#include <chrono>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

#include <instrumental/settings.h>

#include "trace_writer.h"

namespace srv
{
namespace tracer
{

TraceWriter::TraceWriter(const TracerSettings& settings, std::shared_ptr<IDateProvider> dateProvider)
    : m_writer(&TraceWriter::Run, this)
    , m_dateProvider(std::move(dateProvider))
{
    SetSettings(settings);
}

TraceWriter::~TraceWriter() noexcept
{
    // Stop writer
    m_stop = true;
    m_writerCv.notify_all();
    m_writer.join();
}

void TraceWriter::SetSettings(const TracerSettings& settings)
{
    std::lock_guard lock(m_settingsMutex);

    ufa::TryExtractFromOptional(settings.maxTraceLevelForConsole, m_maxLevelForConsole);
    ufa::TryExtractFromOptional(settings.minMessagesToProcess, m_minMessagesToProcess);
    ufa::TryExtractFromOptional(settings.processTimeoutMs, m_processTimeoutMs);

    if (settings.traceFolder.has_value())
    {
        SetFolder(settings.traceFolder.value());
    }
}

void TraceWriter::SetFolder(std::filesystem::path traceFolder)
{
    m_traceFile = traceFolder / GetFilename();
    PrepareFile();
}

void TraceWriter::Queue(std::unique_ptr<ITraceMessage> traceMessage)
{
    {
        std::lock_guard lock(m_messagesMutex);
        m_messagesQueue.emplace(std::move(traceMessage));
    }
    m_writerCv.notify_all();
}

void TraceWriter::Run()
{
    while (true)
    {
        std::unique_lock lock(m_messagesMutex);

        if (m_stop && m_messagesQueue.empty())
        {
            break;
        }

        uint32_t processTimeoutMs;
        uint32_t minMessagesToProcess;
        {
            std::lock_guard lock(m_settingsMutex);
            processTimeoutMs = m_processTimeoutMs;
            minMessagesToProcess = m_minMessagesToProcess;
        }

        m_writerCv.wait_for(lock,
            std::chrono::milliseconds(processTimeoutMs),
            [this, minMessagesToProcess]() -> bool
            {
                return m_messagesQueue.size() >= minMessagesToProcess || m_stop;
            });

        if (!m_messagesQueue.empty())
        {
            auto messageQueue = std::move(m_messagesQueue);
            lock.unlock();

            ProcessMessages(std::move(messageQueue));
        }
    }
}

void TraceWriter::ProcessMessages(std::queue<std::unique_ptr<ITraceMessage>> messages)
{
    std::ofstream fileStream;

    {
        std::lock_guard lock(m_settingsMutex);
        fileStream.open(m_traceFile, std::ios::app);

        if (!fileStream.good())
        {
            PrepareFile();
            fileStream.open(m_traceFile, std::ios::app);
        }
    }

    while (!messages.empty())
    {
        auto messagePtr = std::move(messages.front());
        messages.pop();

        WriteToStream(*messagePtr, fileStream);

        if (messagePtr->GetTraceLevel() <= m_maxLevelForConsole)
        {
            WriteToStream(*messagePtr, std::cout);
        }
    }
}

void TraceWriter::WriteToStream(const ITraceMessage& message, std::ostream& stream)
{
    stream << message.ToString() << std::endl;
}

void TraceWriter::PrintHeader(std::ostream& stream) const
{
    stream << "Trace file begin" << std::endl;
}

void TraceWriter::PrepareFile()
{
    // firstly try to create file
    std::ofstream fileStream;
    fileStream.open(m_traceFile, std::ios::app | std::ios::trunc);
    fileStream.close();

    // check if it was successful
    fileStream.open(m_traceFile, std::ios::app);

    if (!fileStream.good())
    {
        fileStream.close();

        // if not - reset to current folder
        std::error_code error;
        m_traceFile = std::filesystem::current_path(error) / m_traceFile.filename();

        fileStream.open(m_traceFile, std::ios::app | std::ios::trunc);
        fileStream.close();

        fileStream.open(m_traceFile, std::ios::app);
    }

    PrintHeader(fileStream);
}

std::string TraceWriter::GetFilename() const
{
    std::string timestamp = m_dateProvider->GetTimeString();
    std::replace(std::begin(timestamp), std::end(timestamp), ':', '.');

    const std::string fileSuffix = "_was.log";
    return timestamp + fileSuffix;
}

}  // namespace tracer
}  // namespace srv