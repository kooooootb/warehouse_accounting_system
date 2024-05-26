#ifndef H_7B67306B_DEBA_437F_9642_FA08C676F92B
#define H_7B67306B_DEBA_437F_9642_FA08C676F92B

#define TRACE(traceLevel) GetTracer()->StartCollecting(traceLevel)

#define TRACE_ALW TRACE(srv::tracer::TraceLevel::ALWAYS)
#define TRACE_CRT TRACE(srv::tracer::TraceLevel::CRITICAL)
#define TRACE_ERR TRACE(srv::tracer::TraceLevel::ERROR)
#define TRACE_WRN TRACE(srv::tracer::TraceLevel::WARNING)
#define TRACE_INF TRACE(srv::tracer::TraceLevel::INFO)
#define TRACE_DBG TRACE(srv::tracer::TraceLevel::DEBUG)

#define TRACE_HEADER " [" << __func__ << "] "

#endif  // H_7B67306B_DEBA_437F_9642_FA08C676F92B