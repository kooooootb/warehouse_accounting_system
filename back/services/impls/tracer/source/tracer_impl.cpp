#include <ifaces/service_locator.h>

#include "tracer_impl.h"

namespace srv {
namespace tracer {

TracerImpl::TracerImpl(srv::IServiceLocator* serviceLocator) : hldr::ServiceLocatorHolder(serviceLocator), m_writer(&TracerImpl::runWriter, this) {}

TracerImpl::TraceCollector TracerImpl::StartCollecting(ufa::TraceLevel traceLevel) {}

}  // namespace tracer
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::ITracer, srv::tracer::TracerImpl)
