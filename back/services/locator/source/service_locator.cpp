#include <config_reader/config_reader.h>
#include <date_provider/date_provider.h>
#include <environment/environment.h>
#include <settings_provider/settings_provider.h>
#include <tracer/tracer.h>

#include "service_locator.h"

namespace srv
{

ServiceLocator::ServiceLocator() {}

ufa::Result IServiceLocator::Create(std::shared_ptr<srv::IServiceLocator>& object)
{
    object = std::make_unique<ServiceLocator>();
    return ufa::Result::SUCCESS;
}

void ServiceLocator::RegisterDefaults()
{
    Setup();
}

void ServiceLocator::Setup()
{
    // CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IEnvironment>()); // should be already registered
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IConfigReader>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::IDateProvider>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::ISettingsProvider>());
    CHECK_SUCCESS(TryRegisterDefaultInterface<srv::ITracer>());
}

std::shared_ptr<srv::IService> ServiceLocator::GetInterfaceImpl(srv::iid_t iid)
{
    const auto ifaceIt = m_ifaceStorage.find(iid);

    return ifaceIt == m_ifaceStorage.cend() ? nullptr : ifaceIt->second;
}

ufa::Result ServiceLocator::RegisterInterfaceImpl(std::shared_ptr<srv::IService> object, srv::iid_t iid)
{
    const auto pair = m_ifaceStorage.insert(std::make_pair(iid, std::move(object)));

    return pair.second ? ufa::Result::SUCCESS : ufa::Result::REREGISTERING_INTERFACE;
}

}  // namespace srv
