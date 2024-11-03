#ifndef H_4EBDDD15_39D5_4A31_8CE4_A8EEEF39BA9A
#define H_4EBDDD15_39D5_4A31_8CE4_A8EEEF39BA9A

#include <gmock/gmock.h>

#include <locator/service_locator.h>

namespace srv
{

struct ServiceLocatorMock : public IServiceLocator
{
    MOCK_METHOD(void, Setup, (), (override));
    MOCK_METHOD(std::shared_ptr<srv::IService>, GetInterfaceImpl, (srv::iid_t iid), (const, override));
    MOCK_METHOD(ufa::Result, RegisterInterfaceImpl, (std::shared_ptr<srv::IService> object, srv::iid_t iid), (override));
    MOCK_METHOD(std::shared_ptr<IServiceLocator>, GetSharedFromThis, (), (override));
};

}  // namespace srv

#endif  // H_4EBDDD15_39D5_4A31_8CE4_A8EEEF39BA9A