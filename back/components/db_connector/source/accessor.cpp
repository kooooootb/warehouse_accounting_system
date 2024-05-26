#include <instrumental/common.h>
#include <tracer/tracer_provider.h>

#include "accessor.h"

namespace db
{

Accessor::Accessor(std::shared_ptr<srv::IServiceLocator> locator) : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
}

std::unique_ptr<IAccessor> IAccessor::Create(std::shared_ptr<srv::IServiceLocator> locator)
{
    return std::make_unique<Accessor>(std::move(locator));
}

ufa::Result Accessor::FillUser(data::User& user)
{
    return ufa::Result::NOT_IMPLEMENTED;
}

}  // namespace db