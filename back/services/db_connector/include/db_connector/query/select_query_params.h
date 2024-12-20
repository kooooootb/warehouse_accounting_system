#ifndef H_62C23D0F_670E_4963_A983_B475CD0B9E87
#define H_62C23D0F_670E_4963_A983_B475CD0B9E87

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/query_options.h>
#include <instrumental/serialized_enum.h>
#include <instrumental/string_converters.h>

#include "condition.h"
#include "join.h"

DEFINE_ENUM_WITH_SERIALIZATION(srv::db, OrderType, ASC, DESC);

namespace srv
{
namespace db
{

struct SelectOptions : public IQueryOptions
{
    QueryIdentificator GetIdentificator() const override
    {
        return QueryIdentificator::SELECT;
    }

    std::string SerializeParametrized(placeholder_t& placeholders) override
    {
        using namespace std::literals;

        std::string result;

        CHECK_TRUE(table != Table::Invalid, "Invalid query options");

        if (columns.empty())
        {
            result += fmt::format("SELECT * FROM public.\"{}\""sv, string_converters::ToString(table));
        }
        else
        {
            std::vector<std::string> columnsStrs;
            columnsStrs.reserve(columns.size());

            for (auto column : columns)
            {
                bool localize = false;
                for (const auto& join : joins)
                {
                    if (join.leftColumn == column)
                    {
                        localize = true;
                        break;
                    }
                }

                if (localize)
                {
                    columnsStrs.emplace_back(
                        fmt::format("public.\"{}\".{}"sv, string_converters::ToString(table), string_converters::ToString(column)));
                }
                else
                {
                    columnsStrs.emplace_back(string_converters::ToString(column));
                }
            }

            result += fmt::format("SELECT {} FROM public.\"{}\""sv,
                string_converters::ToString(std::begin(columnsStrs), std::end(columnsStrs), ", "sv),
                string_converters::ToString(table));
        }

        for (const auto& join : joins)
        {
            result += fmt::format(" {} JOIN public.\"{}\" ON public.\"{}\".{}=public.\"{}\".{}"sv,
                string_converters::ToString(join.type),
                string_converters::ToString(join.joiningTable),
                string_converters::ToString(join.leftTable.value_or(table)),
                string_converters::ToString(join.leftColumn),
                string_converters::ToString(join.joiningTable),
                string_converters::ToString(join.joiningColumn));
        }

        if (condition != nullptr)
        {
            result += fmt::format(" WHERE {}"sv, condition->ToString(placeholders));
        }

        if (orderBy.has_value())
        {
            result +=
                fmt::format(" ORDER BY {} {}"sv, string_converters::ToString(orderBy.value()), string_converters::ToString(orderType));
        }

        if (limit.has_value())
        {
            result += fmt::format(" LIMIT {}"sv, limit.value());
        }

        if (offset.has_value())
        {
            result += fmt::format(" OFFSET {}"sv, offset.value());
        }

        result += ';';

        return result;
    }

    bool Equals(const IQueryOptions& options) override
    {
        if (GetIdentificator() != options.GetIdentificator())
        {
            return false;
        }

        const auto& selectOptions = static_cast<const SelectOptions&>(options);

        return table == selectOptions.table && columns == selectOptions.columns && joins == selectOptions.joins &&
               orderBy == selectOptions.orderBy && limit == selectOptions.limit && offset == selectOptions.offset &&
               ((condition == nullptr && selectOptions.condition == nullptr) ||
                   (condition != nullptr && selectOptions.condition != nullptr && condition->Equals(*selectOptions.condition)));
    }

    Table table = Table::Invalid;           // e.g. .. FROM this
    std::vector<Column> columns;            // e.g. SELECT this FROM ...
    std::vector<Join> joins;                // for all joins
    std::unique_ptr<ICondition> condition;  // can be grouped
    std::optional<Column> orderBy;          // ORDER BY this
    OrderType orderType = OrderType::ASC;   // this
    std::optional<int32_t> limit;           // LIMIT this
    std::optional<int32_t> offset;          // OFFSET this
};

struct SelectValues  // for extendability reasons
{
};

}  // namespace db
}  // namespace srv

#endif  // H_62C23D0F_670E_4963_A983_B475CD0B9E87