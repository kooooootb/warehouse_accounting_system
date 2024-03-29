#ifndef H_A31BCC98_25B8_478D_B49D_B23AF4148D4F
#define H_A31BCC98_25B8_478D_B49D_B23AF4148D4F

#include <memory>

#include <instrumental/interface.h>

namespace srv {

struct IServiceLocator : public ufa::IBase {
public:
    DECLARE_IID(0x37787C77)

    template <class T>
    ufa::Result GetInterface(T** object);

    template <class T>
    ufa::Result RegisterInterface(std::unique_ptr<T> object);

protected:
    virtual ufa::IBase* GetInterfaceImpl(ufa::iid_t iid) = 0;
    virtual ufa::Result RegisterInterfaceImpl(std::unique_ptr<ufa::IBase> object, ufa::iid_t iid) = 0;

    template <class T>
    void RegisterDefaultInterface();
};

template <class T>
ufa::Result IServiceLocator::GetInterface(T** object)
{
    auto* iface = GetInterfaceImpl(GET_IID(T));
    *object = static_cast<T*>(iface);

    return iface != nullptr ? ufa::Result::SUCCESS : ufa::Result::NO_INTERFACE;
}

template <class T>
ufa::Result IServiceLocator::RegisterInterface(std::unique_ptr<T> object)
{
    static_assert(std::is_base_of_v<ufa::IBase, T>, "Type should be derived from IBase");

    return RegisterInterfaceImpl(std::move(object), GET_IID(T));
}

}  // namespace srv

#endif  // H_A31BCC98_25B8_478D_B49D_B23AF4148D4F