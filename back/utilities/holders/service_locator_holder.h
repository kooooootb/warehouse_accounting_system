#ifndef H_B3B0673D_0DBC_4399_9D8B_9DCF2B6F2D21
#define H_B3B0673D_0DBC_4399_9D8B_9DCF2B6F2D21

#include <ifaces/service_locator.h>

namespace hldr {

class ServiceLocatorHolder {
public:
    ServiceLocatorHolder(srv::IServiceLocator* serviceLocator) : m_serviceLocator(serviceLocator) {}

protected:
    srv::IServiceLocator* GetServiceLocator() { return m_serviceLocator; }

private:
    srv::IServiceLocator* m_serviceLocator;
};

}  // namespace hldr

#endif  // H_B3B0673D_0DBC_4399_9D8B_9DCF2B6F2D21