#ifndef H_A31BCC98_25B8_478D_B49D_B23AF4148D4F
#define H_A31BCC98_25B8_478D_B49D_B23AF4148D4F

#include <memory>

#include <instrumental/check.h>
#include <instrumental/interface.h>

#include "service.h"

namespace srv
{

struct IServiceLocator : ufa::IBase
{
public:
    DECLARE_IID(0x37787C77)

    template <class T>
    ufa::Result GetInterface(std::shared_ptr<T>& object) const;

    template <class T>
    std::shared_ptr<T> GetInterface() const;

    template <class T>
    ufa::Result RegisterInterface(std::shared_ptr<T> object);

    /**
     * @brief register default common services
     * If other interface is already registered with some default interfaces's id
     * then don't register this one default interface
     */
    virtual void RegisterDefaults() = 0;

    /**
     * @brief client services implement this template to include it in default initialization
     * return shared pointer to newly added service
     */
    template <class T>
    std::shared_ptr<T> RegisterDefaultInterface();

    static ufa::Result Create(std::shared_ptr<srv::IServiceLocator>& object);

protected:
    virtual std::shared_ptr<srv::IService> GetInterfaceImpl(srv::iid_t iid) const = 0;
    virtual ufa::Result RegisterInterfaceImpl(std::shared_ptr<srv::IService> object, srv::iid_t iid) = 0;
    virtual std::shared_ptr<IServiceLocator> GetSharedFromThis() = 0;
};

template <class T>
ufa::Result IServiceLocator::GetInterface(std::shared_ptr<T>& object) const
{
    auto iface = GetInterfaceImpl(GET_IID(T));
    object = std::static_pointer_cast<T>(iface);

    return iface != nullptr ? ufa::Result::SUCCESS : ufa::Result::NO_INTERFACE;
}

template <class T>
std::shared_ptr<T> IServiceLocator::GetInterface() const
{
    auto iface = std::static_pointer_cast<T>(GetInterfaceImpl(GET_IID(T)));
    CHECK_TRUE(iface != nullptr);
    return iface;
}

template <class T>
ufa::Result IServiceLocator::RegisterInterface(std::shared_ptr<T> object)
{
    static_assert(std::is_base_of_v<srv::IService, T>, "Type should be derived from IService");

    return RegisterInterfaceImpl(std::move(object), GET_IID(T));
}

#define DECLARE_DEFAULT_INTERFACE(IfaceType, ImplType)                                     \
    template <>                                                                            \
    std::shared_ptr<IfaceType> srv::IServiceLocator::RegisterDefaultInterface<IfaceType>() \
    {                                                                                      \
        auto _impl = std::make_shared<ImplType>(GetSharedFromThis());                      \
                                                                                           \
        srv::IServiceLocator::RegisterInterface(_impl);                                    \
        return std::move(_impl);                                                           \
    }

}  // namespace srv

#endif  // H_A31BCC98_25B8_478D_B49D_B23AF4148D4F