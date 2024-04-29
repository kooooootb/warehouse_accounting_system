#include <stdexcept>

#include <instrumental/common.h>
#include <settings_provider/settings_provider.h>

#include "trace_message.h"
#include "tracer.h"

namespace srv
{
namespace tracer
{

Tracer::Tracer(IServiceLocator* serviceLocator)
{
    CHECK_SUCCESS(serviceLocator->GetInterface(m_dateProvider));

    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(serviceLocator->GetInterface(settingsProvider));

    TracerSettings tracerSettings;
    settingsProvider->FillSettings(&tracerSettings);

    SetSettings(std::move(tracerSettings));
}

TraceCollectorProxy Tracer::StartCollecting(TraceLevel traceLevel)
{
    auto traceMessage = std::make_unique<TraceMessage>(index++, traceLevel, m_dateProvider);
    return TraceCollectorProxy(shared_from_this(), std::move(traceMessage), traceLevel >= m_maxTraceLevel);
}

void Tracer::SetSettings(TracerSettings&& settings)
{
    const auto oldLevel = m_maxTraceLevel;

    if (settings.traceLevel.has_value())
    {
        m_maxTraceLevel = settings.traceLevel.value();
    }

    if (m_maxTraceLevel == srv::tracer::TraceLevel::DISABLED)
    {
        // turn off tracing
        m_traceWriter = nullptr;
    }
    else if (oldLevel == srv::tracer::TraceLevel::DISABLED && m_maxTraceLevel != srv::tracer::TraceLevel::DISABLED)
    {
        // we turn on tracer and run new tracewriter
        m_traceWriter = std::make_unique<TraceWriter>(std::move(settings.traceFolder.value_or("")));
    }
    else if (settings.traceFolder.has_value())
    {
        // we already have running tracewriter and can set its folder
        CHECK_TRUE(m_traceWriter);  // sane check, we should always have it here
        m_traceWriter->SetFolder(std::move(settings.traceFolder.value()));
    }
}

bool Tracer::IsTracing() const
{
    return m_maxTraceLevel > TraceLevel::DISABLED;
}

void Tracer::Trace(std::unique_ptr<ITraceMessage> traceMessage, TraceLevel traceLevel)
{
    if (m_traceWriter != nullptr)
        m_traceWriter->Queue(traceMessage->ToString());
}

}  // namespace tracer
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::ITracer, srv::tracer::Tracer);