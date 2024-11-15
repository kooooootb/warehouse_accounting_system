#ifndef H_789A15D4_5BBA_46CD_8266_7646219F5354
#define H_789A15D4_5BBA_46CD_8266_7646219F5354

#include <fmt/core.h>

#include <instrumental/check.h>
#include <instrumental/serialized_enum.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>

#include <db_connector/product_definitions/columns.h>

#include "utilities.h"

DEFINE_ENUM_WITH_SERIALIZATION(srv::db, ConditionType, Group, Real, In, Not);
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
    NotEqual,
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
        case ::srv::db::RealConditionType::NotEqual:
            return "!=";
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
    virtual ConditionType GetType() const = 0;

    /**
     * @param placeholders will be used to generate placeholders, serializes only const options part
     */
    virtual std::string ToString(placeholder_t& placeholders) const = 0;

    /**
     * @brief collects all dynamic params from conditions
     */
    virtual void CollectParams(params_t& params) const = 0;

    /**
     * @brief compare only part used in ToString
     */
    virtual bool Equals(const ICondition& condition) const = 0;
};

// AND OR
struct GroupCondition : public ICondition
{
    ConditionType GetType() const override
    {
        return ConditionType::Group;
    }

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

    bool Equals(const ICondition& condition) const override
    {
        if (GetType() != condition.GetType())
        {
            return false;
        }

        const auto& groupCondition = static_cast<const GroupCondition&>(condition);

        if (conditions.size() != groupCondition.conditions.size())
        {
            return false;
        }

        for (size_t i = 0; i < conditions.size(); ++i)
        {
            if (!conditions[i]->Equals(*groupCondition.conditions[i]))
            {
                return false;
            }
        }

        return true;
    }

    GroupConditionType type;
    std::vector<std::unique_ptr<ICondition>> conditions;
};

template <typename ValueT>
struct RealCondition : public ICondition
{
    RealCondition(Column column_, ValueT value_, RealConditionType type_) : type(type_), column(column_), value(value_) {}

    ConditionType GetType() const override
    {
        return ConditionType::Real;
    }

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
        params.Append(value);
    }

    bool Equals(const ICondition& condition) const override
    {
        if (GetType() != condition.GetType())
        {
            return false;
        }

        // this can cause problems as we convert to current template type
        // but value field is last so we shouldn't meet invalid memory fields
        // and certain column should store value of certain type
        // and we dont check value here
        const auto& realCondition = static_cast<const RealCondition&>(condition);

        return type == realCondition.type && column == realCondition.column;
    }

    RealConditionType type;
    Column column;
    ValueT value;
};

template <typename ValueT>
struct InCondition : public ICondition  // column IN (...)
{
    InCondition(Column column_, std::vector<ValueT> values_) : column(column_), values(std::move(values_)) {}

    ConditionType GetType() const override
    {
        return ConditionType::In;
    }

    std::string ToString(placeholder_t& placeholders) const override
    {
        using namespace std::literals;

        CHECK_TRUE(!values.empty());

        std::vector<std::string> placeholdersVector;
        placeholdersVector.reserve(values.size());

        for (size_t i = 0; i < values.size(); ++i)
        {
            placeholdersVector.emplace_back(placeholders.get());
            placeholders.next();
        }

        auto result = fmt::format("({} IN ({}))"sv,
            string_converters::ToString(column),
            string_converters::ToString(std::cbegin(placeholdersVector), std::cend(placeholdersVector), ", "sv));

        return result;
    }

    void CollectParams(params_t& params) const override
    {
        for (const auto& value : values)
        {
            params.Append(value);
        }
    }

    bool Equals(const ICondition& condition) const override
    {
        if (GetType() != condition.GetType())
        {
            return false;
        }

        // this can cause problems as we convert to current template type
        // but value field is last so we shouldn't meet invalid memory fields
        // and certain column should store value of certain type
        // and we dont check value here
        const auto& inCondition = static_cast<const InCondition&>(condition);

        return column == inCondition.column && values.size() == inCondition.values.size();
    }

    Column column;
    std::vector<ValueT> values;
};

struct NotCondition : public ICondition
{
    ConditionType GetType() const override
    {
        return ConditionType::Not;
    }

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

    bool Equals(const ICondition& condition) const override
    {
        if (GetType() != condition.GetType())
        {
            return false;
        }

        const auto& notCondition = static_cast<const NotCondition&>(condition);

        return condition.Equals(*notCondition.condition);
    }

    std::unique_ptr<ICondition> condition;
};

inline std::unique_ptr<GroupCondition> CreateGroupCondition(GroupConditionType type)
{
    auto condition = std::make_unique<GroupCondition>();
    condition->type = type;
    return condition;
}

template <typename ValueT>
inline std::unique_ptr<RealCondition<ValueT>> CreateRealCondition(Column column,
    ValueT value,
    RealConditionType type = RealConditionType::Equal)
{
    return std::make_unique<RealCondition<ValueT>>(column, value, type);
}

template <typename ValueT>
inline std::unique_ptr<InCondition<ValueT>> CreateInCondition(Column column, std::vector<ValueT> values)
{
    return std::make_unique<InCondition<ValueT>>(column, std::move(values));
}

}  // namespace db
}  // namespace srv

#endif  // H_789A15D4_5BBA_46CD_8266_7646219F5354