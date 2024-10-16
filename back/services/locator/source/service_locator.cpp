#include <memory>

#include <authorizer/authorizer.h>
#include <config_reader/config_reader.h>
#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <document_manager/document_manager.h>
#include <environment/environment.h>
#include <locator/service_locator.h>
#include <settings_provider/settings_provider.h>
#include <tracer/tracer.h>
#include <tracer/tracer_lazy_provider.h>

#include "service_locator.h"

namespace srv
{

ServiceLocator::ServiceLocator() : srv::tracer::TracerLazyProvider(weak_from_this())
{
    TRACE_INF << TRACE_HEADER;  // doesn't have much sense
}

ufa::Result IServiceLocator::Create(std::shared_ptr<srv::IServiceLocator>& object)
{
    object = std::make_shared<ServiceLocator>();
    return ufa::Result::SUCCESS;
}

void ServiceLocator::RegisterDefaults()
{
    TRACE_INF << TRACE_HEADER;

    Setup();
}

void ServiceLocator::Setup()
{
    TRACE_INF << TRACE_HEADER;

    // order is pretty much important because services depend on each other
    // can resolve it through dependency graph but it is too time-cost
    TryRegisterDefaultInterface<srv::IEnvironment>();  // should be already registered
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IConfigReader>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IDateProvider>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::ISettingsProvider>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::ITracer>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IAccessor>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IAuthorizer>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IDocumentManager>());
}

std::shared_ptr<srv::IService> ServiceLocator::GetInterfaceImpl(srv::iid_t iid) const
{
    const auto ifaceIt = m_ifaceStorage.find(iid);

    if (ifaceIt != m_ifaceStorage.cend())
    {
        TRACE_INF << TRACE_HEADER << "Returning interface with iid: " << iid;
        return ifaceIt->second;
    }

    TRACE_WRN << TRACE_HEADER << "Couldn't find interface with iid: " << iid;
    return nullptr;
}

ufa::Result ServiceLocator::RegisterInterfaceImpl(std::shared_ptr<srv::IService> object, srv::iid_t iid)
{
    const auto pair = m_ifaceStorage.insert(std::make_pair(iid, std::move(object)));

    if (pair.second)
    {
        TRACE_INF << TRACE_HEADER << "Registered interface with iid: " << iid;
        return ufa::Result::SUCCESS;
    }

    TRACE_WRN << TRACE_HEADER << "Already registered interface with iid: " << iid;
    return ufa::Result::REREGISTERING_INTERFACE;
}

std::shared_ptr<IServiceLocator> ServiceLocator::GetSharedFromThis()
{
    return std::static_pointer_cast<IServiceLocator>(shared_from_this());
}

}  // namespace srv
