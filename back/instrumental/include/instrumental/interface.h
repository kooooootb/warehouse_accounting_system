#ifndef H_EA56490C_CF38_4161_85F1_C3FB03A1DD69
#define H_EA56490C_CF38_4161_85F1_C3FB03A1DD69

#include <memory>

#include "types.h"

namespace ufa {

/**
 * @brief base interface struct
 */
struct IBase {
    virtual ~IBase() = default;
};

/**
 * @brief interface id tools
 */
#define GET_IID(Type) Type::_iid
#define DECLARE_IID(iid) constexpr static ufa::iid_t _iid = iid;

template <typename Interface>
std::shared_ptr<Interface> CreateDefaultImpl()
{
    static_assert(false, "No default implementation for interface");
}

}  // namespace ufa

#endif  // H_EA56490C_CF38_4161_85F1_C3FB03A1DD69