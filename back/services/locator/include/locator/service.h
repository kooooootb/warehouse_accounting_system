#ifndef H_6616DEBE_8BA3_4650_93E3_FF9A315D14A6
#define H_6616DEBE_8BA3_4650_93E3_FF9A315D14A6

#include <memory>

#include <instrumental/interface.h>

namespace srv
{

using iid_t = int32_t;

/**
 * @brief base service interface
 */
struct IService
{
    IService() = default;
    virtual ~IService() = default;

    IService(const IService&) = delete;
    IService(IService&&) = delete;
};

/**
 * @brief interface id tools
 */
#define GET_IID(Type) Type::_iid
#define DECLARE_IID(iid) constexpr static srv::iid_t _iid = iid;

}  // namespace srv

#endif  // H_6616DEBE_8BA3_4650_93E3_FF9A315D14A6