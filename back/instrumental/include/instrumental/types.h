#ifndef H_BD52BD5A_F72A_4609_96CB_58F69390593C
#define H_BD52BD5A_F72A_4609_96CB_58F69390593C

#include <cstdint>

#include "string_converters.h"

namespace ufa
{

enum class Result : int32_t
{
    SUCCESS = 0,                      // no error
    ERROR = 1,                        // general error, dont use if more suitable error exists
    NO_INTERFACE = 0x800,             // if no such interface available
    REREGISTERING_INTERFACE = 0x801,  // if trying to reregister interface
    NOT_FOUND = 0x1600,               // not found
    NO_STRING_CONVERTER = 0x8000,     // specifically for converter at the bottom
    WRONG_FORMAT = 0x16000,           // met wrong input format
    DUPLICATE = 0x32000,              // something duplicated
    UNAUTHORIZED = 0x48000,           // unauthorized access
    NOT_IMPLEMENTED = 0x64000         // some interface do'nt have implementation yet
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

    return ::ufa::Result::NO_STRING_CONVERTER;
}

}  // namespace string_converters

#endif  // H_BD52BD5A_F72A_4609_96CB_58F69390593C