#ifndef H_62C23D0F_670E_4963_A983_B475CD0B9E87
#define H_62C23D0F_670E_4963_A983_B475CD0B9E87

#include <optional>

#include <tracer/tracer.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/query_options.h>

#include "condition.h"
#include "join.h"

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

        CHECK_TRUE(table != Table::Invalid);

        if (columns.empty())
        {
            result += fmt::format("SELECT * FROM public.\"{}\""sv, string_converters::ToString(table));
        }
        else
        {
            result += fmt::format("SELECT {} FROM public.\"{}\""sv,
                string_converters::ToString(std::begin(columns), std::end(columns), ", "sv),
                string_converters::ToString(table));
        }

        for (const auto& join : joins)
        {
            result += fmt::format(" {} JOIN {} ON {}.{}={}.{}"sv,
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

        // in most situations this will be enough
        if (table != selectOptions.table || columns != selectOptions.columns || joins != selectOptions.joins || id != selectOptions.id)
        {
            return false;
        }

        placeholder_t ph1;
        placeholder_t ph2;
        if (condition->ToString(ph1) != selectOptions.condition->ToString(ph2))  // im too lazy providing this logic
        {
            return false;
        }

        return true;
    }

    Table table = Table::Invalid;  // e.g. .. FROM this
    std::vector<Column> columns;   // e.g. SELECT this FROM ...
    std::vector<Join> joins;       // for all joins
    Column id;
    std::unique_ptr<ICondition> condition;  // can be grouped
};

struct SelectValues  // for extendability reasons
{
};

/*
SELECT columns FROM table;
*/

/*
SELECT columns
FROM table
INNER JOIN join.joiningTable ON leftTable.leftColumn=joiningTable.joiningColumn;
*/

/*
UPDATE table
SET columns={}
WHERE id = {};
*/

/*
INSERT INTO table (columns)
VALUES ({});
*/

/*
DELETE FROM table WHERE id={};
*/

}  // namespace db
}  // namespace srv

#endif  // H_62C23D0F_670E_4963_A983_B475CD0B9E87