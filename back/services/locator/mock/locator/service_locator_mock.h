#ifndef H_4EBDDD15_39D5_4A31_8CE4_A8EEEF39BA9A
#define H_4EBDDD15_39D5_4A31_8CE4_A8EEEF39BA9A

#include <gmock/gmock.h>

#include <locator/service_locator.h>

namespace srv
{
namespace mock
{

struct ServiceLocatorMock : public IServiceLocator
{
    MOCK_METHOD(void, Setup, (), (override));
    MOCK_METHOD(std::shared_ptr<srv::IService>, GetInterfaceImpl, (srv::iid_t iid), (const, override));
    MOCK_METHOD(ufa::Result, RegisterInterfaceImpl, (std::shared_ptr<srv::IService> object, srv::iid_t iid), (override));
    MOCK_METHOD(std::shared_ptr<IServiceLocator>, GetSharedFromThis, (), (override));

    template <typename IMockT>
    void RegisterMock(std::shared_ptr<IMockT> mock)
    {
        using namespace testing;

        EXPECT_CALL(*this, GetInterfaceImpl(Eq(GET_IID(IMockT))))
            .WillRepeatedly(Invoke(
                [mock](srv::iid_t iid)
                {
                    return mock;
                }));
    }

    static std::shared_ptr<ServiceLocatorMock> Create()
    {
        using namespace testing;

        auto locator = std::make_shared<srv::mock::ServiceLocatorMock>();

        return locator;
    }
};

}  // namespace mock
}  // namespace srv

#endif  // H_4EBDDD15_39D5_4A31_8CE4_A8EEEF39BA9A