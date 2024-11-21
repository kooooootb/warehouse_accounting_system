#ifndef H_8BB42A07_19FE_44D0_805C_50FF8D6D6F64
#define H_8BB42A07_19FE_44D0_805C_50FF8D6D6F64

#include <instrumental/string_converters.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/query_options.h>

#include "condition.h"
#include "utilities.h"

namespace srv
{
namespace db
{

struct DeleteOptions : public IQueryOptions
{
    QueryIdentificator GetIdentificator() const override
    {
        return QueryIdentificator::DELETE;
    }

    std::string SerializeParametrized(placeholder_t& placeholders) override
    {
        using namespace std::literals;

        CHECK_TRUE(table != Table::Invalid, "Invalid query options");
        CHECK_TRUE(condition != nullptr, "DELETE with empty condition is prohibited");

        std::string result = fmt::format("DELETE FROM public.\"{}\" WHERE {};"sv,
            string_converters::ToString(table),
            condition->ToString(placeholders));

        return result;
    }

    bool Equals(const IQueryOptions& options) override
    {
        if (GetIdentificator() != options.GetIdentificator())
        {
            return false;
        }

        const auto& deleteOptions = static_cast<const DeleteOptions&>(options);

        return table == deleteOptions.table &&
               ((condition == nullptr && deleteOptions.condition == nullptr) ||
                   (condition != nullptr && deleteOptions.condition != nullptr && condition->Equals(*deleteOptions.condition)));
    }

    Table table = Table::Invalid;           // DELETE FROM this
    std::unique_ptr<ICondition> condition;  // WHERE this
};

struct DeleteValues  // for extendability
{
};

}  // namespace db
}  // namespace srv

#endif  // H_8BB42A07_19FE_44D0_805C_50FF8D6D6F64