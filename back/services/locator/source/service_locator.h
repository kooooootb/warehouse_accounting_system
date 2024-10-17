#ifndef H_3E34BE93_DE6D_4EDA_9131_8485052DE06B
#define H_3E34BE93_DE6D_4EDA_9131_8485052DE06B

#include <memory>
#include <unordered_map>

#include <instrumental/interface.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>
#include <tracer/tracer_lazy_provider.h>

namespace srv
{

class ServiceLocator : public std::enable_shared_from_this<ServiceLocator>,
                       public srv::tracer::TracerLazyProvider,
                       public IServiceLocator

{
public:
    ServiceLocator();

    void Setup() override;

protected:
    std::shared_ptr<srv::IService> GetInterfaceImpl(srv::iid_t iid) const override;
    ufa::Result RegisterInterfaceImpl(std::shared_ptr<srv::IService> object, srv::iid_t iid) override;
    std::shared_ptr<IServiceLocator> GetSharedFromThis() override;

private:
    /**
     * @brief try register default interface if it is not already in map
     * @return ufa::Result SUCCESS on success, REREGISTERING_INTERFACE otherwise
     */
    template <class T>
    ufa::Result TryRegisterDefaultInterface();

    void RegisterDefaults();

private:
    std::unordered_map<srv::iid_t, std::shared_ptr<srv::IService>> m_ifaceStorage;
};

template <class T>
ufa::Result ServiceLocator::TryRegisterDefaultInterface()
{
    TRACE_INF << TRACE_HEADER << "Registering iface with id: " << GET_IID(T);

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