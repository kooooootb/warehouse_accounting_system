#ifndef H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1
#define H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1

#include <atomic>

#include <date_provider/date_provider.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "trace_writer.h"

namespace srv
{
namespace tracer
{

class Tracer : public ITracer, public std::enable_shared_from_this<ITracer>
{
public:
    Tracer(const std::shared_ptr<IServiceLocator>& locator);

    TraceCollectorProxy StartCollecting(TraceLevel traceLevel) override;
    void SetSettings(const TracerSettings& settings) override;
    void Trace(std::unique_ptr<tracer::ITraceMessage> traceMessage) override;

private:
    bool IsTracing() const;

private:
    std::unique_ptr<TraceWriter> m_traceWriter;
    TraceLevel m_maxTraceLevel = TraceLevel::DISABLED;
    std::mutex m_settingsMutex;

    std::atomic<uint64_t> m_index = 0;
    std::shared_ptr<IDateProvider> m_dateProvider;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1