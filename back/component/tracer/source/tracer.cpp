#include <stdexcept>

#include "tracer.h"

namespace srv
{
namespace tracer
{
namespace detail
{

Tracer::Tracer(TraceSettings settings)
{
    if (settings.traceLevel.has_value())
    {
        m_maxTraceLevel = settings.traceLevel.value();
        if (m_maxTraceLevel > TraceLevel::DISABLED)
        {
            if (settings.traceFolder.has_value())
            {
                m_traceWriter = std::make_unique<TraceWriter>(settings.traceFolder.value());
            }
            else
            {
                throw std::invalid_argument("no folder specified in tracer ctor");
            }
        }
    }
    else
    {
        throw std::invalid_argument("no trace level specified in tracer ctor");
    }
}

TraceCollectorProxy Tracer::StartCollecting(TraceLevel traceLevel)
{
    return TraceCollectorProxy(shared_from_this(), traceLevel, traceLevel >= m_maxTraceLevel);
}

void Tracer::SetSettings(TraceSettings settings)
{
    if (settings.traceFolder.has_value() && m_traceWriter != nullptr)
        m_traceWriter->SetFolder(settings.traceFolder.value());

    if (settings.traceLevel.has_value())
        m_maxTraceLevel = settings.traceLevel.value();
}

void Tracer::Trace(std::unique_ptr<detail::ITraceMessage> traceMessage, TraceLevel traceLevel)
{
    if (m_traceWriter != nullptr)
        m_traceWriter->Queue(traceMessage->ToString());
}

}  // namespace detail
}  // namespace tracer
}  // namespace srv
