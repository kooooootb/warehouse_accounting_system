#ifndef H_279EA6B0_A913_4430_A483_0A5FAAA2F146
#define H_279EA6B0_A913_4430_A483_0A5FAAA2F146

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <locator/service.h>

#include "trace_level.h"
#include "trace_macros.h"

namespace srv
{

struct ITracer;

namespace tracer
{

/**
 * @brief trace settings for changing tracer at start and on go
 */
class TracerSettings : public ufa::settings::SettingsBase
{
public:
    SETTINGS_INIT(TracerSettings)
    {
        SETTINGS_INIT_FIELD(traceLevel);
        SETTINGS_INIT_FIELD(traceFolder);
    }

    SETTINGS_FIELD(traceLevel, TraceLevel);
    SETTINGS_FIELD(traceFolder, std::string);
};

using index_t = uint32_t;

struct ITraceMessage : public ufa::IBase
{
    /**
     * @brief add data to front of message (e.g. timestamp)
     */
    virtual void AddToFront(std::string data) = 0;
    /**
     * @brief add data to back of message (e.g. user data)
     */
    virtual void AddToBack(std::string data) = 0;
    /**
     * @brief stringize
     */
    virtual std::string ToString() const = 0;
};

struct ITraceCollector : public ufa::IBase
{
    /**
     * @brief consume message and store it
     */
    virtual void AddMessage(std::string&& message) = 0;

    /**
     * @brief copy message and store
     */
    virtual void AddMessage(const char* message) = 0;
};

/**
 * @brief forward trace collect calls to implementation if not nullptr
 */
class TraceCollectorProxy
{
public:
    TraceCollectorProxy(std::unique_ptr<ITraceCollector> traceCollector) : m_traceCollector(std::move(traceCollector)) {}

    template <typename SourceT>
    TraceCollectorProxy& operator<<(SourceT&& message)
    {
        if (m_traceCollector != nullptr)
            m_traceCollector->AddMessage(string_converters::ToString(std::forward<SourceT>(message)));

        return *this;
    }

    TraceCollectorProxy& operator<<(const char* message)
    {
        if (m_traceCollector != nullptr)
            m_traceCollector->AddMessage(message);

        return *this;
    }

    TraceCollectorProxy& operator<<(std::string_view message)
    {
        if (m_traceCollector != nullptr)
            m_traceCollector->AddMessage(message.data());

        return *this;
    }

private:
    std::unique_ptr<ITraceCollector> m_traceCollector;
};

}  // namespace tracer

/**
 * @brief safely save logs while executing for debugging
 */
struct ITracer : public srv::IService
{
    DECLARE_IID(0x37787C77)

    /**
     * @brief public method for casual tracing
     */
    virtual tracer::TraceCollectorProxy StartCollecting(tracer::TraceLevel traceLevel) = 0;
    /**
     * @brief Set the Settings object
     */
    virtual void SetSettings(tracer::TracerSettings&& settings) = 0;

    /**
     * @brief inner method for straight tracing
     */
    virtual void Trace(std::unique_ptr<tracer::ITraceMessage> traceMessage) = 0;
};

#define LOCAL_TRACER(_tracer)                                                               \
    class _LocalTracer                                                                      \
    {                                                                                       \
    public:                                                                                 \
        _LocalTracer(std::shared_ptr<srv::ITracer> tracer) : m_tracer(std::move(tracer)) {} \
        std::shared_ptr<srv::ITracer> operator()()                                          \
        {                                                                                   \
            return m_tracer;                                                                \
        }                                                                                   \
                                                                                            \
    private:                                                                                \
        std::shared_ptr<srv::ITracer> m_tracer;                                             \
    };                                                                                      \
    _LocalTracer GetTracer(_tracer);

}  // namespace srv

#endif  // H_279EA6B0_A913_4430_A483_0A5FAAA2F146