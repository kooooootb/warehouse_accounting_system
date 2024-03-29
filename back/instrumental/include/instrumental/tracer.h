#ifndef H_F4BB2D83_0F99_4E1C_9889_E4825F5DEB5C
#define H_F4BB2D83_0F99_4E1C_9889_E4825F5DEB5C

#include "types.h"

#define TRACE(traceLevel) GetTracer().StartCollecting(traceLevel)

#define TRACE_ALW TRACE(ufa::TraceLevel::ALWAYS)
#define TRACE_CRT TRACE(ufa::TraceLevel::CRITICAL)
#define TRACE_ERR TRACE(ufa::TraceLevel::ERROR)
#define TRACE_WRN TRACE(ufa::TraceLevel::WARNING)
#define TRACE_INF TRACE(ufa::TraceLevel::INFO)
#define TRACE_DBG TRACE(ufa::TraceLevel::DEBUG)

#endif  // H_F4BB2D83_0F99_4E1C_9889_E4825F5DEB5C