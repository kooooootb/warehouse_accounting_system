#ifndef H_F4B054CB_C431_4A82_A6D7_47004AE77F80
#define H_F4B054CB_C431_4A82_A6D7_47004AE77F80

#include <instrumental/common.h>
#include <instrumental/serialized_enum.h>

namespace srv
{
namespace db
{

struct IQueryOptions
{
    /**
     * @brief get unique query implementation identificator
     */
    virtual QueryIdentificator GetIdentificator() = 0;

    /**
     * @brief serialize query in parametrized string
     * @return pair->first - count of parameters
     * @return pair->second - parametrized string
     */
    virtual std::pair<std::string, uint32_t> SerializeParametrized() = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_F4B054CB_C431_4A82_A6D7_47004AE77F80