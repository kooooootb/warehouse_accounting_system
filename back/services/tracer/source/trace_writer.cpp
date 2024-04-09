#include "trace_writer.h"
#include <chrono>
#include <filesystem>
#include <mutex>
#include <thread>

namespace srv
{
namespace tracer
{

TraceWriter::TraceWriter(std::filesystem::path traceFolder, std::shared_ptr<IDateProvider> dateProvider)
    : m_writer(&TraceWriter::Run, this)
    , m_dateProvider(std::move(dateProvider))
{
    SetFolder(traceFolder);
}

TraceWriter::~TraceWriter() noexcept
{
    // Stopping writer
    m_stop = true;
    m_writerCv.notify_all();
    m_writer.join();

    // Closing file
    {
        // all operation should be stopped as writer gone but nevertheless
        std::lock_guard lock(m_fileMutex);
        m_fileStream.close();
    }
}

void TraceWriter::SetFolder(std::filesystem::path traceFolder)
{
    m_traceFile = traceFolder / GetFilename();

    std::ofstream ofStream(m_traceFile);
    PrintHeader(ofStream);

    {
        std::lock_guard lock(m_fileMutex);
        m_fileStream = std::move(ofStream);
    }
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
    while (!m_stop)
    {
        std::unique_lock lock(m_messagesMutex);

        m_writerCv.wait(lock,
            [this]() -> bool
            {
                return !m_messagesQueue.empty() || m_stop;
            });

        if (!m_messagesQueue.empty())
        {
            auto message = std::move(m_messagesQueue.front());
            m_messagesQueue.pop();
            lock.unlock();

            WriteToFile(std::move(message));
        }
    }
}

void TraceWriter::WriteToFile(std::unique_ptr<ITraceMessage> traceMessage)
{
    std::lock_guard fileLock(m_fileMutex);

    if (m_fileStream.bad())
    {
        m_fileStream.open(m_traceFile, std::ios::app);
        if (m_fileStream.bad())
        {
            SetFolder(std::filesystem::current_path());
        }
    }

    m_fileStream << traceMessage->ToString();
    m_fileStream.flush();
}

void TraceWriter::PrintHeader(std::ostream& stream) const
{
    stream << "Trace file begin" << std::endl;
}

std::string TraceWriter::GetFilename() const
{
    std::string timestamp = m_dateProvider->GetTimeString();
    std::replace(std::begin(timestamp), std::end(timestamp), ':', '.');

    const std::string fileSuffix = "_rms_logfile.log";
    return timestamp + fileSuffix;
}

}  // namespace tracer
}  // namespace srv