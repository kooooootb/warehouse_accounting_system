#include <memory>
#include <string>

#include "trace_collector.h"

namespace srv
{
namespace tracer
{

TraceCollector::TraceCollector(TraceLevel traceLevel, std::shared_ptr<ITracer> tracer) : m_traceLevel(traceLevel), m_tracer(tracer) {}

TraceCollector::~TraceCollector() noexcept
{
    m_tracer->Trace(std::move(m_traceMessage), m_traceLevel);
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

std::unique_ptr<ITraceCollector> CreateTraceCollector(TraceLevel traceLevel, std::shared_ptr<ITracer> tracer)
{
    return std::make_unique<TraceCollector>(traceLevel, std::move(tracer));
}

}  // namespace tracer
}  // namespace srv
