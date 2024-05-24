#ifndef H_3E34BE93_DE6D_4EDA_9131_8485052DE06B
#define H_3E34BE93_DE6D_4EDA_9131_8485052DE06B

#include <memory>
#include <unordered_map>

#include <instrumental/interface.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>

namespace srv
{

class ServiceLocator : public IServiceLocator
{
public:
    ServiceLocator();

    void RegisterDefaults() override;

protected:
    std::shared_ptr<srv::IService> GetInterfaceImpl(srv::iid_t iid) override;
    ufa::Result RegisterInterfaceImpl(std::shared_ptr<srv::IService> object, srv::iid_t iid) override;

private:
    /**
     * @brief try register default interface if it is not already in map
     * @return ufa::Result SUCCESS on success, REREGISTERING_INTERFACE otherwise
     */
    template <class T>
    ufa::Result TryRegisterDefaultInterface();

    void Setup();

private:
    std::unordered_map<srv::iid_t, std::shared_ptr<srv::IService>> m_ifaceStorage;
};

template <class T>
ufa::Result ServiceLocator::TryRegisterDefaultInterface()
{
    const auto iid = GET_IID(T);
    const auto it = m_ifaceStorage.find(iid);
    if (it == m_ifaceStorage.cend())
    {
        RegisterDefaultInterface<T>();
        return ufa::Result::SUCCESS;
    }
    else
    {
        return ufa::Result::REREGISTERING_INTERFACE;
    }
}

}  // namespace srv

#endif  // H_3E34BE93_DE6D_4EDA_9131_8485052DE06B