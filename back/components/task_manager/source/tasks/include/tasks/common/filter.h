#ifndef H_1E8B069B_1D88_4145_A157_801D9F6F9693
#define H_1E8B069B_1D88_4145_A157_801D9F6F9693

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

#include <date_provider/date_provider.h>
#include <db_connector/product_definitions/columns.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/utilities.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

namespace taskmgr
{
namespace tasks
{

inline std::unique_ptr<srv::db::ICondition> ParseFilters(std::shared_ptr<srv::ITracer> tracer,
    srv::IDateProvider& dateProvider,
    const nlohmann::json& filters)
{
    using namespace srv::db;

    constexpr std::string_view KEY_KEY = "key";
    constexpr std::string_view TYPE_KEY = "type";
    constexpr std::string_view VALUE_KEY = "value";

    LOCAL_TRACER(tracer);

    try
    {
        auto grouped = CreateGroupCondition(GroupConditionType::AND);

        for (const auto& filter : filters)
        {
            auto column = string_converters::FromString<Column>(filter.at(KEY_KEY).get<std::string>());
            auto type = string_converters::FromString<RealConditionType>(filter.at(TYPE_KEY).get<std::string>());
            auto valueJson = filter.at(VALUE_KEY);

            if (valueJson.is_string())
            {
                const auto valueString = valueJson.get<std::string>();
                if (type == RealConditionType::Equal && valueString == "NULL")
                {
                    grouped->conditions.emplace_back(CreateIsNullCondition(column));
                }
                else if (timestamp_t timestamp; dateProvider.FromIsoTimeString(valueString, timestamp) == ufa::Result::SUCCESS)
                {
                    grouped->conditions.emplace_back(CreateRealCondition(column, timestamp, type));
                }
            }
            else
            {
                const auto value = valueJson.get<int64_t>();
                grouped->conditions.emplace_back(CreateRealCondition(column, value, type));
            }
        }

        return grouped;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << ex.what();
    }

    return nullptr;
}

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_1E8B069B_1D88_4145_A157_801D9F6F9693