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
#include <tracer/tracer.h>

namespace srv
{
namespace tracer
{

/**
 * @brief only write string to file, no trace logic
 */
class TraceWriter
{
public:
    TraceWriter(std::filesystem::path traceFolder, std::shared_ptr<IDateProvider> dateProvider);

    ~TraceWriter() noexcept;

    void SetFolder(std::filesystem::path traceFolder);
    void Queue(std::unique_ptr<ITraceMessage> traceMessage);

    void Stop();

private:
    // decorations
    void PrintHeader(std::ostream& stream) const;
    std::string GetFilename() const;

    // worker jobs
    void Run();
    void WriteToFile(std::unique_ptr<ITraceMessage> traceMessage);

private:
    // writer
    std::thread m_writer;
    std::condition_variable m_writerCv;
    std::atomic<bool> m_stop{false};

    // messages
    std::queue<std::unique_ptr<ITraceMessage>> m_messagesQueue;
    std::mutex m_messagesMutex;

    // files
    std::filesystem::path m_traceFile;
    std::mutex m_fileMutex;
    std::ofstream m_fileStream;

    // dependecies
    std::shared_ptr<IDateProvider> m_dateProvider;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_10A713D0_81F4_480B_AB52_96A8218C7052