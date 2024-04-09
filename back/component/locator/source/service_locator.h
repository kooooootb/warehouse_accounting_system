#ifndef H_3E34BE93_DE6D_4EDA_9131_8485052DE06B
#define H_3E34BE93_DE6D_4EDA_9131_8485052DE06B

#include <map>
#include <memory>

#include <ifaces/service_locator.h>
#include <instrumental/interface.h>
#include <instrumental/types.h>

namespace srv {

class ServiceLocator : public IServiceLocator {
public:
    ServiceLocator();

protected:
    ufa::IBase* GetInterfaceImpl(ufa::iid_t iid) override;
    ufa::Result RegisterInterfaceImpl(std::unique_ptr<ufa::IBase> object, ufa::iid_t iid) override;

private:
    void Setup();

private:
    std::map<ufa::iid_t, std::unique_ptr<ufa::IBase>> m_ifaceStorage;
};

}  // namespace srv

#endif  // H_3E34BE93_DE6D_4EDA_9131_8485052DE06B