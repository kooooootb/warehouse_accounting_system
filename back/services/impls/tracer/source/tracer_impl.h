#ifndef H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1
#define H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <holders/service_locator_holder.h>
#include <ifaces/service_locator.h>
#include <ifaces/tracer.h>

#include "trace_message.h"

namespace srv {
namespace tracer {

class TracerImpl : public srv::ITracer, public hldr::ServiceLocatorHolder {
public:
    TracerImpl(srv::IServiceLocator* serviceLocator);

    TraceCollector Trace(ufa::TraceLevel traceLevel) override;

private:
    class TraceCollector {
    public:
        TraceCollector& operator<<(const char*&& message);
    };

private:
    void runWriter();

    void QueueMessage(TraceMessage message);
    void FormatMessage(TraceMessage& message);

private:
    std::thread m_writer;
    std::condition_variable m_writerCv;

    std::queue<TraceMessage> m_messagesQueue;
    std::mutex m_messagesMutex;

    std::atomic<bool> m_stopFlag;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1