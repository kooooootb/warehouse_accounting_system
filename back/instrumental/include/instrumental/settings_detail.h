#ifndef H_318467D8_8CBB_4BD1_90B4_A9C53E2E0347
#define H_318467D8_8CBB_4BD1_90B4_A9C53E2E0347

#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace ufa
{

namespace settings
{

class SettingsBase
{
public:
    using Name = std::string_view;
    using Value = std::string;
    using Filler = std::function<bool(Name, Value)>;
    using Getter = std::function<Value()>;

    const std::vector<Name>& GetFields() const
    {
        return m_fieldNames;
    }

    void FillSettings(const std::map<Name, Value>& values)
    {
        std::for_each(std::cbegin(values), std::cend(values),
            [this](const auto& field)
            {
                for (const Filler& filler : m_fillers)
                {
                    if (filler(field.first, field.second))
                        break;
                }
            });
    }

    std::vector<Value> GetValues() const
    {
        std::vector<Value> values;
        values.reserve(m_getters.size());

        for (const auto& getter : m_getters)
        {
            values.emplace_back(getter());
        }

        return values;
    }

protected:
    void AddFiller(Filler&& filler)
    {
        m_fillers.emplace_back(std::move(filler));
    }

    void AddFieldName(Name&& fieldName)
    {
        m_fieldNames.emplace_back(std::move(fieldName));
    }

    void AddGetter(Getter&& getter)
    {
        m_getters.emplace_back(std::move(getter));
    }

private:
    std::vector<Name> m_fieldNames;
    std::vector<Filler> m_fillers;
    std::vector<Getter> m_getters;  // returns value converted to string
};

}  // namespace settings
}  // namespace ufa

#endif  // H_318467D8_8CBB_4BD1_90B4_A9C53E2E0347