#ifndef H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1
#define H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1

#include <tracer/tracer.h>

#include "trace_writer.h"

namespace srv
{
namespace tracer
{
namespace detail
{

class Tracer : public ITracer
{
public:
    Tracer(TraceSettings settings);

    TraceCollectorProxy StartCollecting(TraceLevel traceLevel) override;
    void SetSettings(TraceSettings settings) override;
    void Trace(std::unique_ptr<detail::ITraceMessage> traceMessage, TraceLevel traceLevel) override;

private:
    void runWriter();

    void QueueMessage(std::unique_ptr<ITraceMessage> message);
    void FormatMessage(std::unique_ptr<ITraceMessage>& message);

private:
    std::unique_ptr<TraceWriter> m_traceWriter;
    TraceLevel m_maxTraceLevel;
};

}  // namespace detail
}  // namespace tracer
}  // namespace srv

#endif  // H_CE50211F_FC7E_4799_9C4E_77E496F2C3F1