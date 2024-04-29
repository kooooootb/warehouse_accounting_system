#ifndef H_BB4F8FFF_2AD4_42E7_B55D_F313131E7CFA
#define H_BB4F8FFF_2AD4_42E7_B55D_F313131E7CFA

#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "string_converters.h"

#define SETTINGS_FIELD(_fieldName, _type) std::optional<_type> _fieldName;

#define SETTINGS_DEFAULT_FILLER(_fieldName) string_converters::FromString<decltype(_fieldName)::value_type>

#define SETTINGS_SET_FILLER(_fieldName, _converter)                            \
    AddFiller(                                                                 \
        [this](const NameType& fieldName, const ValueType& fieldValue) -> bool \
        {                                                                      \
            if (fieldName == #_fieldName)                                      \
            {                                                                  \
                this->_fieldName = _converter(fieldValue);                     \
                return true;                                                   \
            }                                                                  \
            return false;                                                      \
        });

#define SETTINGS_SET_GETTER(_fieldName) \
    AddGetter(                          \
        []() -> NameType                \
        {                               \
            return #_fieldName;         \
        });

#define SETTINGS_INIT_FIELD_WITH_FILLER(_fieldName, _converter) \
    SETTINGS_SET_FILLER(_fieldName, _converter);                \
    SETTINGS_SET_GETTER(_fieldName);

#define SETTINGS_INIT_FIELD_WITHOUT_FILLER(_fieldName)                    \
    SETTINGS_SET_FILLER(_fieldName, SETTINGS_DEFAULT_FILLER(_fieldName)); \
    SETTINGS_SET_GETTER(_fieldName);

#define SETTINGS_INIT_FIELD_SWITCHER_GET(_fieldName, arg1, macro, ...) macro
#define SETTINGS_INIT_FIELD_SWITCHER(...) SETTINGS_INIT_FIELD_SWITCHER_GET(__VA_ARGS__, SETTINGS_INIT_FIELD_WITH_FILLER, SETTINGS_INIT_FIELD_WITHOUT_FILLER)

#define SETTINGS_INIT_FIELD(...) SETTINGS_INIT_FIELD_SWITCHER(__VA_ARGS__)(__VA_ARGS__);

namespace ufa
{

namespace settings
{

namespace converters
{

template <typename To>
To Convert(const std::string& from)
{
    return ::string_converters::FromString<To>(from);
}

}  // namespace converters

class SettingsBase
{
public:
    using NameType = std::string;
    using ValueType = std::string;
    using FillerType = std::function<bool(NameType, ValueType)>;
    using GetterType = std::function<NameType()>;

    std::vector<NameType> GetFields() const
    {
        std::vector<NameType> fieldNames;
        fieldNames.reserve(m_getters.size());

        std::for_each(std::cbegin(m_getters), std::cend(m_getters),
            [&fieldNames](const GetterType& getter)
            {
                fieldNames.emplace_back(getter());
            });

        return fieldNames;
    }

    void FillSettings(std::map<NameType, ValueType> values)
    {
        std::for_each(std::cbegin(values), std::cend(values),
            [this](const auto& field)
            {
                for (const FillerType& filler : m_fillers)
                {
                    if (filler(field.first, field.second))
                        break;
                }
            });
    }

protected:
    void AddFiller(FillerType&& filler)
    {
        m_fillers.emplace_back(std::move(filler));
    }

    void AddGetter(GetterType&& getter)
    {
        m_getters.emplace_back(std::move(getter));
    }

    void InitField() {}

private:
    std::vector<GetterType> m_getters;
    std::vector<FillerType> m_fillers;
};

}  // namespace settings
}  // namespace ufa

#endif  // H_BB4F8FFF_2AD4_42E7_B55D_F313131E7CFA