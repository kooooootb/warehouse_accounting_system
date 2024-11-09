#ifndef H_6850FD1E_2620_4732_9663_03B50CD70AE4
#define H_6850FD1E_2620_4732_9663_03B50CD70AE4

#include <algorithm>
#include <iterator>
#include <numeric>
#include <string>
#include <type_traits>

#include "settings_detail.h"

namespace string_converters
{

// for non numeric and non derived from SettingsBase
template <typename FromT>
inline std::enable_if_t<
    !std::is_arithmetic_v<FromT> &&
        (!std::is_base_of_v<ufa::settings::SettingsBase, std::decay_t<FromT>> ||
            std::is_same_v<std::decay_t<FromT>, ufa::settings::SettingsBase>)&&!std::is_same_v<std::decay_t<FromT>, std::string>,
    std::string>
ToString(FromT from)
{
    return std::string(from);
}

// for all integers except bool
template <typename FromT>
inline std::enable_if_t<std::is_integral_v<FromT> && !std::is_same_v<FromT, bool>, std::string> ToString(FromT from)
{
    return std::to_string(from);
}

// for all floating point
template <typename FromT>
inline std::enable_if_t<std::is_floating_point_v<FromT>, std::string> ToString(FromT from)
{
    return std::to_string(from);
}

// for bool
template <typename FromT>
inline std::enable_if_t<std::is_same_v<FromT, bool>, std::string> ToString(FromT from)
{
    return from ? "true" : "false";
}

// for std::string & and &&
template <typename FromT>
inline std::enable_if_t<std::is_same_v<std::decay_t<FromT>, std::string>, std::string> ToString(FromT&& from)
{
    return std::forward<FromT>(from);
}

template <typename ToT>
inline ToT FromString(const std::string& from)
{
    return from;
}

template <>
inline long double FromString(const std::string& from)
{
    return std::stold(from);
}

template <>
inline double FromString(const std::string& from)
{
    return std::stod(from);
}

template <>
inline float FromString(const std::string& from)
{
    return std::stof(from);
}

template <>
inline unsigned long long FromString(const std::string& from)
{
    return std::stoull(from);
}

template <>
inline unsigned long FromString(const std::string& from)
{
    return std::stoul(from);
}

template <>
inline unsigned int FromString(const std::string& from)
{
    return std::stoul(from);
}

template <>
inline long long FromString(const std::string& from)
{
    return std::stoll(from);
}

template <>
inline long FromString(const std::string& from)
{
    return std::stol(from);
}

template <>
inline int FromString(const std::string& from)
{
    return std::stoi(from);
}

template <>
inline bool FromString(const std::string& from)
{
    return !from.empty() && from.front() == 't' ? true : false;  // return false when empty or "f*"
}

template <typename T>
std::string ToString(T begin, T end, std::string_view separator)
{
    std::vector<std::string> results;
    results.reserve(std::distance(begin, end));
    size_t resultSize = 0;

    for (; begin != end; ++begin)
    {
        results.emplace_back(ToString(*begin));
        resultSize += results.back().size();
    }

    std::string result;
    result.reserve(resultSize);

    for (auto it = std::cbegin(results); it != std::prev(std::cend(results)); ++it)
    {
        result.append(*it);
        result.append(separator);
    }
    result.append(*std::prev(std::cend(results)));

    return result;
}

}  // namespace string_converters

#endif  // H_6850FD1E_2620_4732_9663_03B50CD70AE4