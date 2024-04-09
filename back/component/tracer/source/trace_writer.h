#ifndef H_10A713D0_81F4_480B_AB52_96A8218C7052
#define H_10A713D0_81F4_480B_AB52_96A8218C7052

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <tracer/tracer.h>

namespace srv
{
namespace tracer
{
namespace detail
{

/**
 * @brief only write string to file, no trace logic
 */
class TraceWriter
{
public:
    TraceWriter(std::filesystem::path traceFolder);

    void SetFolder(std::filesystem::path traceFolder);
    void Queue(std::string message);

private:
    // writer
    std::thread m_writer;
    std::condition_variable m_writerCv;
    std::atomic<bool> m_stopFlag;

    // messages
    std::queue<std::unique_ptr<ITraceMessage>> m_messagesQueue;
    std::mutex m_messagesMutex;

    // settings
    std::filesystem::path m_traceFile;
};

}  // namespace detail
}  // namespace tracer
}  // namespace srv

#endif  // H_10A713D0_81F4_480B_AB52_96A8218C7052