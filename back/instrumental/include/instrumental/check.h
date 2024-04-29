#ifndef H_DAC5609F_E690_4C8B_B978_BADD9E7556C5
#define H_DAC5609F_E690_4C8B_B978_BADD9E7556C5

#include <sstream>
#include <stdexcept>

#include "string_converters.h"
#include "types.h"

#define CHECK_MAKE_MSG() _message << __FILE__ << '(' << __LINE__ << ") : " << __func__;

#define CHECK_MAKE_MSG_1_ARGS(stream) \
    CHECK_MAKE_MSG()                  \
    _message << stream;

#define CHECK_MAKE_GENERATOR_MSG(arg1, arg2, macro, ...) macro
#define CHECK_GENERATOR_MSG(...) CHECK_MAKE_GENERATOR_MSG(__VA_ARGS__, CHECK_MAKE_MSG_1_ARGS, CHECK_MAKE_MSG)

#define CHECK_SUCCESS_INTERNAL(__generator, x, ...) \
    if (x != ufa::Result::SUCCESS)                  \
    {                                               \
        std::stringstream _message;                 \
        _message << string_converters::ToString(x); \
        __generator(__VA_ARGS__);                   \
        throw std::runtime_error(_message.str());   \
    }

#define CHECK_SUCCESS(...) CHECK_SUCCESS_INTERNAL(CHECK_GENERATOR_MSG(__VA_ARGS__), __VA_ARGS__)

#define CHECK_TRUE_INTERNAL(__generator, x, ...)  \
    if (!!x != true)                              \
    {                                             \
        std::stringstream _message;               \
        __generator(__VA_ARGS__);                 \
        throw std::runtime_error(_message.str()); \
    }

#define CHECK_TRUE(...) CHECK_TRUE_INTERNAL(CHECK_GENERATOR_MSG(__VA_ARGS__), __VA_ARGS__)

#endif  // H_DAC5609F_E690_4C8B_B978_BADD9E7556C5