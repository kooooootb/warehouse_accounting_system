#ifndef H_8038150B_6149_411A_AE90_CD9ADAE6E93B
#define H_8038150B_6149_411A_AE90_CD9ADAE6E93B

#include <fmt/core.h>

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

struct UpdateOptions : public IQueryOptions
{
    QueryIdentificator GetIdentificator() const override
    {
        return QueryIdentificator::UPDATE;
    }

    std::string SerializeParametrized(placeholder_t& placeholders) override
    {
        using namespace std::literals;

        CHECK_TRUE(table != Table::Invalid, "Invalid query options table");
        CHECK_TRUE(!columns.empty(), "Invalid query options: empty columns");
        CHECK_TRUE(condition != nullptr, "Invalid query options: empty condition is prohibited");

        std::vector<std::string> modifications;
        modifications.reserve(columns.size());

        for (const auto column : columns)
        {
            modifications.emplace_back(fmt::format("{} = {}"sv, string_converters::ToString(column), placeholders.get()));
            placeholders.next();
        }

        std::string result = fmt::format("UPDATE public.\"{}\" SET {} WHERE {};"sv,
            string_converters::ToString(table),
            string_converters::ToString(std::cbegin(modifications), std::cend(modifications), ", "sv),
            condition->ToString(placeholders));

        return result;
    }

    bool Equals(const IQueryOptions& options) override
    {
        if (GetIdentificator() != options.GetIdentificator())
        {
            return false;
        }

        const auto& updateOptions = static_cast<const UpdateOptions&>(options);

        return table == updateOptions.table && columns == updateOptions.columns &&
               ((condition == nullptr && updateOptions.condition == nullptr) ||
                   (condition != nullptr && updateOptions.condition != nullptr && condition->Equals(*updateOptions.condition)));
    }

    Table table = Table::Invalid;           // UPDATE this
    std::vector<Column> columns;            // SET (this = $1)
    std::unique_ptr<ICondition> condition;  // WHERE this
};

struct UpdateValues
{
    params_t values;  // SET (columns = this)
};

}  // namespace db
}  // namespace srv

#endif  // H_8038150B_6149_411A_AE90_CD9ADAE6E93B