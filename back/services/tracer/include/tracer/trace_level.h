#ifndef H_78A5C53D_3606_4832_9D20_50FA717C020D
#define H_78A5C53D_3606_4832_9D20_50FA717C020D

#include <instrumental/string_converters.h>

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

}  // namespace tracer
}  // namespace srv

namespace string_converters
{

template <>
inline std::string ToString(::srv::tracer::TraceLevel traceLevel)
{
    switch (traceLevel)
    {
        case ::srv::tracer::TraceLevel::DISABLED:
            return "DIS";
        case ::srv::tracer::TraceLevel::ALWAYS:
            return "ALW";
        case ::srv::tracer::TraceLevel::CRITICAL:
            return "CRT";
        case ::srv::tracer::TraceLevel::ERROR:
            return "ERR";
        case ::srv::tracer::TraceLevel::WARNING:
            return "WRN";
        case ::srv::tracer::TraceLevel::INFO:
            return "INF";
        case ::srv::tracer::TraceLevel::DEBUG:
            return "DBG";
        default:
            return std::to_string(static_cast<int32_t>(traceLevel));
    }
}

template <>
inline ::srv::tracer::TraceLevel FromString(const std::string& string)
{
    if (string == "DIS")
        return ::srv::tracer::TraceLevel::DISABLED;
    if (string == "ALW")
        return ::srv::tracer::TraceLevel::ALWAYS;
    if (string == "CRT")
        return ::srv::tracer::TraceLevel::CRITICAL;
    if (string == "ERR")
        return ::srv::tracer::TraceLevel::ERROR;
    if (string == "WRN")
        return ::srv::tracer::TraceLevel::WARNING;
    if (string == "INF")
        return ::srv::tracer::TraceLevel::INFO;
    if (string == "DBG")
        return ::srv::tracer::TraceLevel::DEBUG;

    return ::srv::tracer::TraceLevel::DEBUG;
}

}  // namespace string_converters

#endif  // H_78A5C53D_3606_4832_9D20_50FA717C020D