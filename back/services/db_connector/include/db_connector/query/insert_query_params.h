#ifndef H_C1315910_2E2C_4A8D_967B_407E7CFAF082
#define H_C1315910_2E2C_4A8D_967B_407E7CFAF082

#include <optional>

#include <db_connector/value_connector.h>
#include <instrumental/string_converters.h>
#include <tracer/tracer.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/query_options.h>

#include "condition.h"
#include "join.h"
#include "utilities.h"

namespace srv
{
namespace db
{

struct InsertOptions : public IQueryOptions
{
    QueryIdentificator GetIdentificator() const override
    {
        return QueryIdentificator::INSERT;
    }

    std::string SerializeParametrized(placeholder_t& placeholders) override
    {
        using namespace std::literals;

        std::string result;

        CHECK_TRUE(table != Table::Invalid, "Invalid query options");

        result += fmt::format("INSERT INTO public.\"{}\" "sv, string_converters::ToString(table));

        if (!columns.empty())
        {
            result += fmt::format("({}) "sv, string_converters::ToString(std::cbegin(columns), std::cend(columns), ", "sv));
        }

        result += "VALUES "sv;

        std::vector<std::string> values;
        values.reserve(valuesCount);
        for (size_t i = 0; i < valuesCount; ++i)
        {
            std::vector<std::string> currentPlaceholders;
            currentPlaceholders.reserve(columns.size());

            for (size_t j = 0; j < columns.size(); ++j)
            {
                currentPlaceholders.emplace_back(placeholders.view());
                placeholders.next();
            }

            values.emplace_back(fmt::format("({})"sv,
                string_converters::ToString(std::cbegin(currentPlaceholders), std::cend(currentPlaceholders), ", "sv)));
        }
        result += string_converters::ToString(std::cbegin(values), std::cend(values), ", "sv);

        result += ';';

        return result;
    }

    bool Equals(const IQueryOptions& options) override
    {
        if (GetIdentificator() != options.GetIdentificator())
        {
            return false;
        }

        const auto& insertOptions = static_cast<const InsertOptions&>(options);

        return table == insertOptions.table && columns == insertOptions.columns && valuesCount == insertOptions.valuesCount;
    }

    Table table = Table::Invalid;  // INSERT INTO this
    std::vector<Column> columns;   // (this)
    size_t valuesCount = 1;        // count inserting rows, should be calculated in factories
};

struct InsertValues
{
    std::vector<params_t> values;  // VALUES (this), size should be == valuesCount
};

}  // namespace db
}  // namespace srv

#endif  // H_C1315910_2E2C_4A8D_967B_407E7CFAF082