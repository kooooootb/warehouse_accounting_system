#ifndef H_DAC5609F_E690_4C8B_B978_BADD9E7556C5
#define H_DAC5609F_E690_4C8B_B978_BADD9E7556C5

#include <sstream>
#include <stdexcept>

#include "string_converters.h"
#include "types.h"

#define CHECK_MAKE_MSG() _message << __FILE__ << '(' << __LINE__ << ") [" << __func__ << "]: ";

#define CHECK_MAKE_MSG_1_ARGS(stream) \
    CHECK_MAKE_MSG()                  \
    _message << ", message: " << stream;

#define CHECK_MAKE_GENERATOR_MSG(arg1, arg2, macro, ...) macro
#define CHECK_GENERATOR_MSG(...) CHECK_MAKE_GENERATOR_MSG(__VA_ARGS__, CHECK_MAKE_MSG_1_ARGS, CHECK_MAKE_MSG)

#define CHECK_SUCCESS_INTERNAL(__generator, x, ...)                                                                                \
    if (x != ufa::Result::SUCCESS)                                                                                                 \
    {                                                                                                                              \
        std::stringstream _message;                                                                                                \
        _message << "CHECK_SUCCESS failed with Result: " << string_converters::ToString(x) << ", expression: [" #x "], in file: "; \
        __generator(__VA_ARGS__);                                                                                                  \
        throw std::runtime_error(_message.str());                                                                                  \
    }

#define CHECK_SUCCESS(...)                                                    \
    do                                                                        \
    {                                                                         \
        CHECK_SUCCESS_INTERNAL(CHECK_GENERATOR_MSG(__VA_ARGS__), __VA_ARGS__) \
    } while (false)

#define CHECK_TRUE_INTERNAL(__generator, x, ...)                          \
    if (!!(x) != true)                                                    \
    {                                                                     \
        std::stringstream _message;                                       \
        _message << "CHECK_TRUE failed, expression: [" #x "], in file: "; \
        __generator(__VA_ARGS__);                                         \
        throw std::runtime_error(_message.str());                         \
    }

#define CHECK_TRUE(...)                                                    \
    do                                                                     \
    {                                                                      \
        CHECK_TRUE_INTERNAL(CHECK_GENERATOR_MSG(__VA_ARGS__), __VA_ARGS__) \
    } while (false)

#endif  // H_DAC5609F_E690_4C8B_B978_BADD9E7556C5