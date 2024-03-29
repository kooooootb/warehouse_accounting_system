#ifndef H_EA56490C_CF38_4161_85F1_C3FB03A1DD69
#define H_EA56490C_CF38_4161_85F1_C3FB03A1DD69

#include "types.h"

namespace ufa {

struct IBase {
    virtual ~IBase() = default;
};

#define DECLARE_IID(iid) constexpr static ufa::iid_t _iid = iid;

#define GET_IID(Type) Type::_iid

#define DECLARE_DEFAULT_INTERFACE(IfaceType, ImplType)               \
    template <>                                                      \
    void srv::IServiceLocator::RegisterDefaultInterface<IfaceType>() \
    {                                                                \
        auto _impl = std::make_unique<ImplType>(this);               \
                                                                     \
        srv::IServiceLocator::RegisterInterface(std::move(_impl));   \
    }

}  // namespace ufa

#endif  // H_EA56490C_CF38_4161_85F1_C3FB03A1DD69