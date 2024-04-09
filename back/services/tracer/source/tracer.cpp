#include <memory>
#include <stdexcept>

#include <instrumental/common.h>
#include <settings_provider/settings_provider.h>

#include "trace_collector.h"
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
    if (traceLevel <= m_maxTraceLevel)
    {
        // We trace
        std::shared_ptr<ITracer> tracer = shared_from_this();
        auto traceMessage = std::make_unique<TraceMessage>(m_index++, traceLevel, m_dateProvider);
        auto traceCollector = std::make_unique<TraceCollector>(std::move(tracer), std::move(traceMessage));
        return TraceCollectorProxy(std::move(traceCollector));
    }
    else
    {
        // We not trace
        return TraceCollectorProxy(nullptr);
    }
}

void Tracer::SetSettings(TracerSettings&& settings)
{
    std::lock_guard lock(m_settingsMutex);

    // Set trace folder
    if (settings.traceFolder.has_value())
    {
        m_traceFolder = std::move(settings.traceFolder.value());

        if (m_traceWriter != nullptr)
        {
            m_traceWriter->SetFolder(std::move(m_traceFolder));
        }
    }

    // Set trace level
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
        m_traceWriter = std::make_unique<TraceWriter>(m_traceFolder, m_dateProvider);
    }
}

bool Tracer::IsTracing() const
{
    return m_maxTraceLevel > TraceLevel::DISABLED;
}

void Tracer::Trace(std::unique_ptr<ITraceMessage> traceMessage)
{
    if (m_traceWriter != nullptr)
        m_traceWriter->Queue(std::move(traceMessage));
}

}  // namespace tracer
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::ITracer, srv::tracer::Tracer);