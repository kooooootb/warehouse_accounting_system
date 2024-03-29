#include "tracer_impl.h"

namespace srv {
namespace tracer {

TracerImpl::TraceCollector& TracerImpl::TraceCollector::operator<<(const char*&& message) {}

}  // namespace tracer
}  // namespace srv
