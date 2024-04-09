#ifndef H_279EA6B0_A913_4430_A483_0A5FAAA2F146
#define H_279EA6B0_A913_4430_A483_0A5FAAA2F146

#include <filesystem>
#include <memory>
#include <optional>
#include <string>

#include <instrumental/interface.h>

namespace srv
{
namespace tracer
{

/**
 * @brief Specify trace message to process and save.
 * Bigger number means more memory and processing consumption and more detailed saved messages
 */
enum class TraceLevel : int32_t
{
    DISABLED = 0,    // no tracing
    ALWAYS = 200,    // message absolutely important for logging
    CRITICAL = 400,  // errors in critical components only
    ERROR = 600,     // errors in common components
    WARNING = 800,   // least significant error messages and important rare scenarios
    INFO = 1000,     // common information
    DEBUG = 1200,    // same as INFO but with big messages that may affect the performance
};

#define TRACE(traceLevel) GetTracer().StartCollecting(traceLevel)

#define TRACE_ALW TRACE(ufa::TraceLevel::ALWAYS)
#define TRACE_CRT TRACE(ufa::TraceLevel::CRITICAL)
#define TRACE_ERR TRACE(ufa::TraceLevel::ERROR)
#define TRACE_WRN TRACE(ufa::TraceLevel::WARNING)
#define TRACE_INF TRACE(ufa::TraceLevel::INFO)
#define TRACE_DBG TRACE(ufa::TraceLevel::DEBUG)

/**
 * @brief trace settings for changing tracer at start and on go, empty fields dont change
 */
struct TraceSettings
{
    std::optional<TraceLevel> traceLevel;
    std::optional<std::filesystem::path> traceFolder;
};

struct ITracer;
std::shared_ptr<ITracer> CreateTracer(TraceSettings settings);

namespace detail
{

struct ITraceMessage;
struct ITraceCollector;

std::unique_ptr<ITraceCollector> CreateTraceCollector(TraceLevel traceLevel, std::shared_ptr<ITracer> tracer);
std::unique_ptr<ITraceMessage> CreateTraceMessage();

struct ITraceMessage : public ufa::IBase
{
    DECLARE_IID(0x8bf3fded)

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
    DECLARE_IID(0x68799e97)

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
 * @brief stores trace level and forward trace collect calls to implementation
 */
class TraceCollectorProxy
{
public:
    TraceCollectorProxy(std::shared_ptr<ITracer> tracer, TraceLevel traceLevel, bool shouldTrace)
    {
        if (shouldTrace)
            m_traceCollector = CreateTraceCollector(traceLevel, tracer);
    }

    template <typename StringType>
    TraceCollectorProxy& operator<<(StringType&& message)
    {
        if (m_traceCollector != nullptr)
            m_traceCollector->AddMessage(std::forward(message));

        return *this;
    }

private:
    std::unique_ptr<ITraceCollector> m_traceCollector;
};

}  // namespace detail

/**
 * @brief safely save logs while executing for debugging
 */
struct ITracer : public ufa::IBase, std::enable_shared_from_this<ITracer>
{
    DECLARE_IID(0x37787C77)

    /**
     * @brief public method for casual tracing
     */
    virtual detail::TraceCollectorProxy StartCollecting(TraceLevel traceLevel) = 0;
    /**
     * @brief Set the Settings object
     * 
     */
    virtual void SetSettings(TraceSettings settings) = 0;

    /**
     * @brief inner method for straight tracing
     */
    virtual void Trace(std::unique_ptr<detail::ITraceMessage> traceMessage, TraceLevel traceLevel) = 0;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_279EA6B0_A913_4430_A483_0A5FAAA2F146