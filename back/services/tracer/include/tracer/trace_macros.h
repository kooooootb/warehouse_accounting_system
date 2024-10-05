#ifndef H_7B67306B_DEBA_437F_9642_FA08C676F92B
#define H_7B67306B_DEBA_437F_9642_FA08C676F92B

#include <string>

#define TRACE(traceLevel)                                                           \
    if (auto _tracer = GetTracer(); _tracer)                                        \
        if (auto _proxy = _tracer->StartCollecting(traceLevel); _proxy.IsTracing()) \
    _proxy

#define TRACE_ALW TRACE(srv::tracer::TraceLevel::ALWAYS)
#define TRACE_CRT TRACE(srv::tracer::TraceLevel::CRITICAL)
#define TRACE_ERR TRACE(srv::tracer::TraceLevel::ERROR)
#define TRACE_WRN TRACE(srv::tracer::TraceLevel::WARNING)
#define TRACE_INF TRACE(srv::tracer::TraceLevel::INFO)
#define TRACE_DBG TRACE(srv::tracer::TraceLevel::DEBUG)

constexpr std::string_view FormatFunctionString(std::string_view functionString)
{
    auto bracketIndex = functionString.find('(');

    auto spaceIndex = functionString.rfind(' ', bracketIndex);
    spaceIndex = spaceIndex == functionString.npos ? 0 : spaceIndex + 1;

    return functionString.substr(spaceIndex, bracketIndex - spaceIndex);
}

#ifndef TRACER_FUNCTION_HEADER
#if defined(__clang__)
#define TRACER_FUNCTION_HEADER FormatFunctionString(__PRETTY_FUNCTION__)
#elif defined(__GNUC__) || defined(__GNUG__)
#define TRACER_FUNCTION_HEADER FormatFunctionString(__PRETTY_FUNCTION__)
#elif defined(_MSC_VER)
#define TRACER_FUNCTION_HEADER FormatFunctionString(__PRETTY_FUNCTION__)
#else
#define TRACER_FUNCTION_HEADER __func__
#endif
#endif  // TRACER_FUNCTION_HEADER

#define TRACE_HEADER " [" << TRACER_FUNCTION_HEADER << "] "

#endif  // H_7B67306B_DEBA_437F_9642_FA08C676F92B