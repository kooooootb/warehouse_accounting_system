#include <memory>
#include <string>

#include "trace_collector.h"

namespace srv
{
namespace tracer
{

TraceCollector::TraceCollector(std::shared_ptr<ITracer> tracer, std::unique_ptr<ITraceMessage> traceMessage)
    : m_tracer(tracer), m_traceMessage(std::move(traceMessage))
{
}

TraceCollector::~TraceCollector() noexcept
{
    m_tracer->Trace(std::move(m_traceMessage));
}

void TraceCollector::AddMessage(std::string&& message)
{
    m_traceMessage->AddToBack(std::move(message));
}

void TraceCollector::AddMessage(const char* message)
{
    std::string messageStr(message);
    m_traceMessage->AddToBack(std::move(messageStr));
}

}  // namespace tracer
}  // namespace srv
