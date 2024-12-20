#ifndef H_BD52BD5A_F72A_4609_96CB_58F69390593C
#define H_BD52BD5A_F72A_4609_96CB_58F69390593C

#include <cstdint>

#include "string_converters.h"

namespace ufa
{

enum class Result : int32_t
{
    SUCCESS = 0,                  // no error
    ERROR = 1,                    // general error, dont use if more suitable error exists
    NO_INTERFACE = 2,             // if no such interface available
    REREGISTERING_INTERFACE = 3,  // if trying to reregister interface
    NOT_FOUND = 4,                // not found
    NO_STRING_CONVERTER = 5,      // specifically for converter at the bottom
    WRONG_FORMAT = 6,             // met wrong input format
    DUPLICATE = 7,                // something duplicated
    UNAUTHORIZED = 8,             // unauthorized access
    NOT_IMPLEMENTED = 9,          // some interface or feature doesn't have implementation yet
    NO_CONNECTION = 10,           // couldn't create connection
    VIOLATION = 11,               // common rules were violated
};

}  // namespace ufa

namespace string_converters
{

template <>
inline std::string ToString(::ufa::Result result)
{
    switch (result)
    {
        case ::ufa::Result::SUCCESS:
            return "SUCCESS";
        case ::ufa::Result::ERROR:
            return "ERROR";
        case ::ufa::Result::NO_INTERFACE:
            return "NO_INTERFACE";
        case ::ufa::Result::REREGISTERING_INTERFACE:
            return "REREGISTERING_INTERFACE";
        case ::ufa::Result::NOT_FOUND:
            return "NOT_FOUND";
        case ::ufa::Result::NO_STRING_CONVERTER:
            return "NO_STRING_CONVERTER";
        case ::ufa::Result::WRONG_FORMAT:
            return "WRONG_FORMAT";
        case ::ufa::Result::DUPLICATE:
            return "DUPLICATE";
        case ::ufa::Result::UNAUTHORIZED:
            return "UNAUTHORIZED";
        case ::ufa::Result::NOT_IMPLEMENTED:
            return "NOT_IMPLEMENTED";
        case ::ufa::Result::NO_CONNECTION:
            return "NO_CONNECTION";
        case ::ufa::Result::VIOLATION:
            return "VIOLATION";
        default:
            return std::to_string(static_cast<int32_t>(result));
    }
}

template <>
inline ::ufa::Result FromString(const std::string& string)
{
    if (string == "SUCCESS")
        return ::ufa::Result::SUCCESS;
    if (string == "ERROR")
        return ::ufa::Result::ERROR;
    if (string == "NO_INTERFACE")
        return ::ufa::Result::NO_INTERFACE;
    if (string == "REREGISTERING_INTERFACE")
        return ::ufa::Result::REREGISTERING_INTERFACE;
    if (string == "NOT_FOUND")
        return ::ufa::Result::NOT_FOUND;
    if (string == "WRONG_FORMAT")
        return ::ufa::Result::WRONG_FORMAT;
    if (string == "DUPLICATE")
        return ::ufa::Result::DUPLICATE;
    if (string == "UNAUTHORIZED")
        return ::ufa::Result::UNAUTHORIZED;
    if (string == "NOT_IMPLEMENTED")
        return ::ufa::Result::NOT_IMPLEMENTED;
    if (string == "NO_CONNECTION")
        return ::ufa::Result::NO_CONNECTION;
    if (string == "VIOLATION")
        return ::ufa::Result::VIOLATION;

    return ::ufa::Result::NO_STRING_CONVERTER;
}

}  // namespace string_converters

#endif  // H_BD52BD5A_F72A_4609_96CB_58F69390593C