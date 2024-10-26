#ifndef H_AFCC5DFB_68A6_4C12_9B2A_DC157D840927
#define H_AFCC5DFB_68A6_4C12_9B2A_DC157D840927

#include "check.h"
#include "types.h"

#define UFA_EVAL0(...) __VA_ARGS__
#define UFA_EVAL1(...) UFA_EVAL0(UFA_EVAL0(UFA_EVAL0(__VA_ARGS__)))
#define UFA_EVAL2(...) UFA_EVAL1(UFA_EVAL1(UFA_EVAL1(__VA_ARGS__)))
#define UFA_EVAL3(...) UFA_EVAL2(UFA_EVAL2(UFA_EVAL2(__VA_ARGS__)))
#define UFA_EVAL4(...) UFA_EVAL3(UFA_EVAL3(UFA_EVAL3(__VA_ARGS__)))
#define UFA_EVAL(...) UFA_EVAL4(UFA_EVAL4(UFA_EVAL4(__VA_ARGS__)))

#define UFA_MAP_END(...)
#define UFA_MAP_OUT
#define UFA_MAP_COMMA ,

#define UFA_MAP_GET_END2() 0, UFA_MAP_END
#define UFA_MAP_GET_END1(...) UFA_MAP_GET_END2
#define UFA_MAP_GET_END(...) UFA_MAP_GET_END1
#define UFA_MAP_NEXT0(test, next, ...) next UFA_MAP_OUT
#define UFA_MAP_NEXT1(test, next) UFA_MAP_NEXT0(test, next, 0)
#define UFA_MAP_NEXT(test, next) UFA_MAP_NEXT1(UFA_MAP_GET_END test, next)

#define UFA_MAP0(f, x, peek, ...) f(x) UFA_MAP_NEXT(peek, UFA_MAP1)(f, peek, __VA_ARGS__)
#define UFA_MAP1(f, x, peek, ...) f(x) UFA_MAP_NEXT(peek, UFA_MAP0)(f, peek, __VA_ARGS__)

#define UFA_MAP_LIST_NEXT1(test, next) UFA_MAP_NEXT0(test, UFA_MAP_COMMA next, 0)
#define UFA_MAP_LIST_NEXT(test, next) UFA_MAP_LIST_NEXT1(UFA_MAP_GET_END test, next)

#define UFA_MAP_LIST0(f, x, peek, ...) f(x) UFA_MAP_LIST_NEXT(peek, UFA_MAP_LIST1)(f, peek, __VA_ARGS__)
#define UFA_MAP_LIST1(f, x, peek, ...) f(x) UFA_MAP_LIST_NEXT(peek, UFA_MAP_LIST0)(f, peek, __VA_ARGS__)

#define UFA_MAP(f, ...) UFA_EVAL(UFA_MAP1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define UFA_MAP_LIST(f, ...) UFA_EVAL(UFA_MAP_LIST1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#define UFA_EWS_TOSTRING(_EnumValue) \
    case EnumType::_EnumValue:       \
        return #_EnumValue;

#define UFA_EWS_FROMSTRING(_EnumValue) \
    if (string == #_EnumValue)         \
        return EnumType::_EnumValue;

#define DEFINE_ENUM_WITH_SERIALIZATION(_EnumNamespace, _EnumName, ...)                         \
    namespace _EnumNamespace                                                                   \
    {                                                                                          \
                                                                                               \
    enum class _EnumName : uint32_t                                                            \
    {                                                                                          \
        __VA_ARGS__                                                                            \
    };                                                                                         \
    } /* namespace _EnumNamespace */                                                           \
                                                                                               \
    namespace string_converters                                                                \
    {                                                                                          \
                                                                                               \
    template <>                                                                                \
    inline std::string ToString(::_EnumNamespace::_EnumName enumer)                            \
    {                                                                                          \
        using EnumType = ::_EnumNamespace::_EnumName;                                          \
        switch (enumer)                                                                        \
        {                                                                                      \
            UFA_MAP(UFA_EWS_TOSTRING, __VA_ARGS__)                                             \
            default:                                                                           \
                CHECK_SUCCESS(ufa::Result::NO_STRING_CONVERTER, static_cast<int32_t>(enumer)); \
                return {};                                                                     \
        }                                                                                      \
    }                                                                                          \
                                                                                               \
    template <>                                                                                \
    inline _EnumNamespace::_EnumName FromString(const std::string& string)                     \
    {                                                                                          \
        using EnumType = ::_EnumNamespace::_EnumName;                                          \
        UFA_MAP(UFA_EWS_FROMSTRING, __VA_ARGS__)                                               \
                                                                                               \
        CHECK_SUCCESS(ufa::Result::NO_STRING_CONVERTER, string);                               \
        return static_cast<EnumType>(0);                                                       \
    }                                                                                          \
    } /* namespace string_converters */

#endif  // H_AFCC5DFB_68A6_4C12_9B2A_DC157D840927