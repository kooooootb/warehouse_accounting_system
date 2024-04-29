#ifndef H_6850FD1E_2620_4732_9663_03B50CD70AE4
#define H_6850FD1E_2620_4732_9663_03B50CD70AE4

#include <string>

namespace string_converters
{

template <typename FromT>
inline std::string ToString(const FromT&) = delete;

template <typename ToT>
inline ToT FromString(const std::string&) = delete;

template <>
inline std::string ToString(const std::string& from)
{
    return from;
}

template <>
inline std::string FromString(const std::string& from)
{
    return from;
}

}  // namespace string_converters

#endif  // H_6850FD1E_2620_4732_9663_03B50CD70AE4