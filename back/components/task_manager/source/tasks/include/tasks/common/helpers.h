#ifndef H_0EBCEA36_F91E_4212_8DD4_66363CEA9779
#define H_0EBCEA36_F91E_4212_8DD4_66363CEA9779

#include <optional>
#include <string>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/query/utilities.h>

namespace taskmgr
{
namespace tasks
{

template <typename T>
void AppendToQueryOptional(std::vector<srv::db::Column>& columns,
    srv::db::params_t& params,
    const std::optional<T>& optionalValue,
    srv::db::Column column)
{
    if (optionalValue.has_value())
    {
        columns.emplace_back(column);
        params.Append(optionalValue.value());
    }
}

template <typename T>
void AppendToQueryNullable(srv::db::params_t& params, const std::optional<T>& optionalValue)
{
    if (optionalValue.has_value())
    {
        params.Append(optionalValue.value());
    }
    else
    {
        params.Append(std::string_view("NULL"));
    }
}

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_0EBCEA36_F91E_4212_8DD4_66363CEA9779