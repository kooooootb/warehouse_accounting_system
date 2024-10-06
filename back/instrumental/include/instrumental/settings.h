#ifndef H_BB4F8FFF_2AD4_42E7_B55D_F313131E7CFA
#define H_BB4F8FFF_2AD4_42E7_B55D_F313131E7CFA

#include "check.h"
#include "settings_detail.h"
#include "string_converters.h"

#define SETTINGS_FIELD(_fieldName, _type) std::optional<_type> _fieldName;

#define SETTINGS_DEFAULT_FROMCONVERTER(_fieldName) string_converters::FromString<decltype(_fieldName)::value_type>

#define SETTINGS_DEFAULT_TOCONVERTER(_fieldName)                                                     \
    string_converters::ToString<std::conditional<std::is_scalar_v<decltype(_fieldName)::value_type>, \
        decltype(_fieldName)::value_type,                                                            \
        const decltype(_fieldName)::value_type&>::type>

#define SETTINGS_SET_FILLER(_fieldName, _from_converter, _to_converter) \
    AddFiller(                                                          \
        [this](const Name& fieldName, const Value& fieldValue) -> bool  \
        {                                                               \
            if (fieldName == #_fieldName)                               \
            {                                                           \
                this->_fieldName = _from_converter(fieldValue);         \
                return true;                                            \
            }                                                           \
            return false;                                               \
        });                                                             \
    AddGetter(                                                          \
        [this]() -> std::string                                         \
        {                                                               \
            if (this->_fieldName.has_value())                           \
                return _to_converter(this->_fieldName.value());         \
            else                                                        \
                return "<unspecified>";                                 \
        });

#define SETTINGS_ADD_FIELDNAME(_fieldName) AddFieldName(#_fieldName);

#define SETTINGS_INIT_FIELD_WITH_CONVERTER(_fieldName, _from_converter, _to_converter) \
    SETTINGS_SET_FILLER(_fieldName, _from_converter, _to_converter);                   \
    SETTINGS_ADD_FIELDNAME(_fieldName);

#define SETTINGS_INIT_FIELD_WITHOUT_CONVERTER(_fieldName)                                                                  \
    SETTINGS_SET_FILLER(_fieldName, SETTINGS_DEFAULT_FROMCONVERTER(_fieldName), SETTINGS_DEFAULT_TOCONVERTER(_fieldName)); \
    SETTINGS_ADD_FIELDNAME(_fieldName);

#define SETTINGS_INIT_FIELD_SWITCHER_GET(_fieldName, arg1, arg2, macro, ...) macro

#define SETTINGS_INIT_FIELD_SWITCHER(...) \
    SETTINGS_INIT_FIELD_SWITCHER_GET(__VA_ARGS__, SETTINGS_INIT_FIELD_WITH_CONVERTER, arg, SETTINGS_INIT_FIELD_WITHOUT_CONVERTER)

#define SETTINGS_INIT_FIELD(...) SETTINGS_INIT_FIELD_SWITCHER(__VA_ARGS__)(__VA_ARGS__);

#define SETTINGS_INIT(_settingsName)                  \
    std::string_view GetSettingsName() const override \
    {                                                 \
        return #_settingsName;                        \
    }                                                 \
    _settingsName()

namespace ufa
{

template <typename T>
inline bool TryExtractFromOptional(std::optional<T> from, T& to)
{
    if (from.has_value())
    {
        to = std::move(from.value());
        return true;
    }

    return false;
}

}  // namespace ufa

namespace string_converters
{

template <>
inline std::string ToString(const ufa::settings::SettingsBase& settings)
{
    auto fieldNames = settings.GetFields();
    auto fieldValues = settings.GetValues();

    std::stringstream result;
    result << '<';

    CHECK_TRUE(fieldNames.size() == fieldValues.size());
    for (size_t i = 0; i < fieldNames.size(); ++i)
    {
        result << fieldNames[i] << " = " << fieldValues[i];
    }

    result << '>';

    return result.str();
}

// for derived from SettingsBase
template <typename FromT>
inline std::enable_if_t<std::is_base_of_v<ufa::settings::SettingsBase, std::decay_t<FromT>> &&
                            !std::is_same_v<std::decay_t<FromT>, ufa::settings::SettingsBase>,
    std::string>
ToString(const FromT& settings)
{
    return ToString<const ufa::settings::SettingsBase&>(settings);
}

}  // namespace string_converters

#endif  // H_BB4F8FFF_2AD4_42E7_B55D_F313131E7CFA