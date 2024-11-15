#ifndef H_84A04B28_7FCA_4C6B_893B_EBF19BBD0E0E
#define H_84A04B28_7FCA_4C6B_893B_EBF19BBD0E0E

#include <optional>

#include <nlohmann/json.hpp>

namespace util
{
namespace json
{

template <typename T>
T Get(const nlohmann::json& json, std::string_view key)
{
    return json.at(key).get<T>();
}

template <typename T>
std::optional<T> GetOptional(const nlohmann::json& json, std::string_view key)
{
    const auto it = json.find(key);
    if (it != json.end())
    {
        return *it;
    }
    else
    {
        return std::nullopt;
    }
}

template <typename T>
void Put(nlohmann::json& json, std::string_view key, T data)
{
    json[key] = std::move(data);
}

// overload optional differently
template <typename T>
void Put(nlohmann::json& json, std::string_view key, std::optional<T> data)
{
    if (data.has_value())
    {
        json[key] = std::move(data.value());
    }
}

}  // namespace json
}  // namespace util

#endif  // H_84A04B28_7FCA_4C6B_893B_EBF19BBD0E0E