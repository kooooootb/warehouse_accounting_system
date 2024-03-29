#include <ifaces/tracer.h>

#include <locator/service_locator_factory.h>

#include "service_locator.h"

namespace srv {

ServiceLocator::ServiceLocator()
{
    Setup();
}

void ServiceLocator::Setup()
{
    RegisterDefaultInterface<srv::ITracer>();
}

ufa::IBase* ServiceLocator::GetInterfaceImpl(ufa::iid_t iid)
{
    const auto ifaceIt = m_ifaceStorage.find(iid);

    return ifaceIt == m_ifaceStorage.cend() ? nullptr : ifaceIt->second.get();
}

ufa::Result ServiceLocator::RegisterInterfaceImpl(std::unique_ptr<ufa::IBase> object, ufa::iid_t iid)
{
    const auto pair = m_ifaceStorage.insert(std::make_pair(iid, std::move(object)));

    return pair.second ? ufa::Result::SUCCESS : ufa::Result::REREGISTERING_INTERFACE;
}

ufa::Result CreateServiceLocator(std::unique_ptr<srv::IServiceLocator>& object)
{
    object = std::make_unique<ServiceLocator>();

    return ufa::Result::SUCCESS;
}

}  // namespace srv
