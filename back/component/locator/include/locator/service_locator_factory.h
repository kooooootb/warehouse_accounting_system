#ifndef H_861C76CA_903E_4D2D_9ABE_5B7F7AD65141
#define H_861C76CA_903E_4D2D_9ABE_5B7F7AD65141

#include <memory>

#include <ifaces/service_locator.h>

namespace srv {

/**
 * @brief Declare default interface implementation used in service locator
 */
#define DECLARE_DEFAULT_INTERFACE(IfaceType, ImplType)               \
    template <>                                                      \
    void srv::IServiceLocator::RegisterDefaultInterface<IfaceType>() \
    {                                                                \
        auto _impl = std::make_unique<ImplType>(this);               \
                                                                     \
        srv::IServiceLocator::RegisterInterface(std::move(_impl));   \
    }

ufa::Result CreateServiceLocator(std::unique_ptr<srv::IServiceLocator>& object);

}  // namespace srv

#endif  // H_861C76CA_903E_4D2D_9ABE_5B7F7AD65141