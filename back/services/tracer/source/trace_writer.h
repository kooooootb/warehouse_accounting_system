#ifndef H_10A713D0_81F4_480B_AB52_96A8218C7052
#define H_10A713D0_81F4_480B_AB52_96A8218C7052

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>

#include <date_provider/date_provider.h>
#include <tracer/trace_level.h>
#include <tracer/tracer.h>

namespace srv
{
namespace tracer
{

/**
 * @brief only write string to file or stdout
 */
class TraceWriter
{
public:
    TraceWriter(const TracerSettings& settings, std::shared_ptr<IDateProvider> dateProvider);

    ~TraceWriter() noexcept;

    void SetSettings(const TracerSettings& settings);

    void Queue(std::unique_ptr<ITraceMessage> traceMessage);

    void Stop();

private:
    // decorations
    void PrintHeader(std::ostream& ostream) const;
    void PrepareFile();  // called under m_settingsMutex
    std::string GetFilename() const;

    // settings
    void SetFolder(std::filesystem::path traceFolder);  // called under m_settingsMutex

    // worker jobs
    void Run();
    void ProcessMessages(std::queue<std::unique_ptr<ITraceMessage>> message);
    void WriteToStream(const ITraceMessage& message, std::ostream& ostream);

private:
    // writer
    std::thread m_writer;
    std::condition_variable m_writerCv;
    std::atomic<bool> m_stop{false};

    // messages
    std::queue<std::unique_ptr<ITraceMessage>> m_messagesQueue;
    std::mutex m_messagesMutex;

    // trace settings
    std::mutex m_settingsMutex;
    std::filesystem::path m_traceFile;

    //filters messages from tracer, so if tracer decided no to trace, it will not be printed in console in anyway
    TraceLevel m_maxLevelForConsole = TraceLevel::ERROR;
    uint32_t m_minMessagesToProcess = 5;
    uint32_t m_processTimeoutMs = 10000;

    // dependencies
    std::shared_ptr<IDateProvider> m_dateProvider;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_10A713D0_81F4_480B_AB52_96A8218C7052