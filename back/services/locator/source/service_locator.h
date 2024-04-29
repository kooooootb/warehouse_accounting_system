#ifndef H_3E34BE93_DE6D_4EDA_9131_8485052DE06B
#define H_3E34BE93_DE6D_4EDA_9131_8485052DE06B

#include <map>
#include <memory>

#include <instrumental/interface.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>

namespace srv
{

class ServiceLocator : public IServiceLocator
{
public:
    ServiceLocator();

protected:
    std::shared_ptr<srv::IService> GetInterfaceImpl(srv::iid_t iid) override;
    ufa::Result RegisterInterfaceImpl(std::shared_ptr<srv::IService> object, srv::iid_t iid) override;

private:
    void Setup();

private:
    std::map<srv::iid_t, std::shared_ptr<srv::IService>> m_ifaceStorage;
};

}  // namespace srv

#endif  // H_3E34BE93_DE6D_4EDA_9131_8485052DE06B