#ifndef H_789A15D4_5BBA_46CD_8266_7646219F5354
#define H_789A15D4_5BBA_46CD_8266_7646219F5354

#include <fmt/core.h>

#include <instrumental/check.h>
#include <instrumental/serialized_enum.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>

#include <db_connector/product_definitions/columns.h>

#include "utilities.h"

DEFINE_ENUM_WITH_SERIALIZATION(srv::db, GroupConditionType, AND, OR);

namespace srv
{
namespace db
{

enum class RealConditionType : int32_t
{
    Equal,
    Less,
    Greater,
    NotLess,
    NotGreater
};

}  // namespace db
}  // namespace srv

namespace string_converters
{

template <>
inline std::string ToString(::srv::db::RealConditionType condition)
{
    switch (condition)
    {
        case ::srv::db::RealConditionType::Equal:
            return "=";
        case ::srv::db::RealConditionType::Less:
            return "<";
        case ::srv::db::RealConditionType::Greater:
            return ">";
        case ::srv::db::RealConditionType::NotLess:
            return ">=";
        case ::srv::db::RealConditionType::NotGreater:
            return "<=";
        default:
            CHECK_SUCCESS(ufa::Result::NO_STRING_CONVERTER);
            return std::to_string(static_cast<int32_t>(condition));
    }
}

template <>
inline ::srv::db::RealConditionType FromString(const std::string& string)
{
    if (string == "=")
        return ::srv::db::RealConditionType::Equal;
    if (string == "<")
        return ::srv::db::RealConditionType::Less;
    if (string == ">")
        return ::srv::db::RealConditionType::Greater;
    if (string == ">=")
        return ::srv::db::RealConditionType::NotLess;
    if (string == "<=")
        return ::srv::db::RealConditionType::NotGreater;

    CHECK_SUCCESS(ufa::Result::NO_STRING_CONVERTER);
    return ::srv::db::RealConditionType::Equal;
}

}  // namespace string_converters

namespace srv
{
namespace db
{

struct ICondition
{
    /**
     * @param placeholders will be used to generate placeholders
     */
    virtual std::string ToString(placeholder_t& placeholders) const = 0;

    /**
     * @brief collects all dynamic params from conditions
     */
    virtual void CollectParams(params_t& params) const = 0;
};

// AND OR
struct GroupCondition : public ICondition
{
    std::string ToString(placeholder_t& placeholders) const override
    {
        using namespace std::literals;
        CHECK_TRUE(!conditions.empty(), "Empty group conditions don't make sense");

        std::string result("("sv);

        for (size_t i = 0; i < conditions.size() - 1; ++i)
        {
            result += fmt::format("{} {} "sv, conditions[i]->ToString(placeholders), string_converters::ToString(type));
        }

        result += fmt::format("{})"sv, conditions.back()->ToString(placeholders));

        return result;
    }

    void CollectParams(params_t& params) const override
    {
        for (const auto& condition : conditions)
        {
            condition->CollectParams(params);
        }
    }

    GroupConditionType type;
    std::vector<std::unique_ptr<ICondition>> conditions;
};

template <typename ValueT>
struct RealCondition : public ICondition
{
    RealCondition(Column column_, ValueT value_, RealConditionType type_) : type(type_), column(column_), value(value_) {}

    std::string ToString(placeholder_t& placeholders) const override
    {
        using namespace std::literals;

        auto result =
            fmt::format("({} {} {})"sv, string_converters::ToString(column), string_converters::ToString(type), placeholders.view());
        placeholders.next();

        return result;
    }

    void CollectParams(params_t& params) const override
    {
        params.append(value);
    }

    RealConditionType type;
    Column column;
    ValueT value;
};

struct NotCondition : public ICondition
{
    std::string ToString(placeholder_t& placeholders) const override
    {
        using namespace std::literals;
        CHECK_TRUE(condition != nullptr, "Not condition is empty");

        std::string result = fmt::format("(NOT {})"sv, condition->ToString(placeholders));

        return result;
    }

    void CollectParams(params_t& params) const override
    {
        condition->CollectParams(params);
    }

    std::unique_ptr<ICondition> condition;
};

}  // namespace db
}  // namespace srv

#endif  // H_789A15D4_5BBA_46CD_8266_7646219F5354