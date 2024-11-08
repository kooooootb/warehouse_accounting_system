#ifndef H_F4B054CB_C431_4A82_A6D7_47004AE77F80
#define H_F4B054CB_C431_4A82_A6D7_47004AE77F80

#include <instrumental/common.h>
#include <instrumental/serialized_enum.h>

#include <tracer/tracer.h>

#include "query_identificator.h"
#include "utilities.h"

namespace srv
{
namespace db
{

struct IQueryOptions : public ufa::IBase
{
    virtual QueryIdentificator GetIdentificator() const = 0;

    /**
     * @brief serialize query in parametrized string
     * @param placeholders will generate placeholders
     * @return parametrized string
     */
    virtual std::string SerializeParametrized(placeholder_t& placeholders) = 0;

    /**
     * @brief serialize query in parametrized string
     * @return parametrized string
     */
    virtual std::string SerializeParametrized()
    {
        placeholder_t placeholders;
        return SerializeParametrized(placeholders);
    }

    /**
     * @brief compare two options
     */
    virtual bool Equals(const IQueryOptions& options) = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_F4B054CB_C431_4A82_A6D7_47004AE77F80