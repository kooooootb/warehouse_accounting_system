#include <environment/environment.h>
#include <instrumental/check.h>
#include <instrumental/common.h>
#include <instrumental/types.h>

#include <algorithm>
#include <cctype>
#include <filesystem>

#include "settings_provider.h"

namespace srv
{

namespace settings_provider
{

namespace
{

constexpr std::string_view EnvironmentKeyPrefix = "RMS";
constexpr std::string_view EnvironmentKeySeparator = "_";

std::string ConvertToEnvironmentKey(std::string_view sectionName, std::string_view key)
{
    std::string result;
    result.reserve(sectionName.size() + key.size() + EnvironmentKeyPrefix.size() + 2 * EnvironmentKeySeparator.size());

    result.append(EnvironmentKeyPrefix);
    result.append(EnvironmentKeySeparator);

    for (auto ch : sectionName)
    {
        result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
    }

    result.append(EnvironmentKeySeparator);

    for (auto ch : key)
    {
        result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(ch))));
    }

    return result;
}

}  // namespace

SettingsProvider::SettingsProvider(const std::shared_ptr<IServiceLocator>& locator) : srv::tracer::TracerLazyProvider(locator)
{
    TRACE_INF << TRACE_HEADER;

    CHECK_SUCCESS(locator->GetInterface(m_configReader));
    CHECK_SUCCESS(locator->GetInterface(m_environment));
}

bool SettingsProvider::TryFromEnvironment(std::string_view settingsName, std::string_view name, std::string& value) const
{
    TRACE_INF << TRACE_HEADER << "Settings name: " << settingsName << ", setting: " << name;
    if (m_environment != nullptr)
    {
        if (m_environment->GetValue(ConvertToEnvironmentKey(settingsName, name), value) == ufa::Result::SUCCESS)
        {
            TRACE_INF << TRACE_HEADER << "Received: " << value;
            return true;
        }
    }

    return false;
}

ufa::Result SettingsProvider::FillSettings(ufa::settings::SettingsBase* settings) const
{
    const auto fields = settings->GetFields();
    const auto settingsName = settings->GetSettingsName();

    std::map<ufa::settings::SettingsBase::Name, ufa::settings::SettingsBase::Value> settingsMap;
    ufa::Result result = ufa::Result::SUCCESS;

    for (const auto& field : fields)
    {
        std::string value;
        auto curResult = ufa::Result::SUCCESS;

        if (!this->TryFromEnvironment(settingsName, field, value))
        {
            curResult = m_configReader->ReadValue({settingsName, field}, value);
        }

        if (curResult != ufa::Result::SUCCESS)
        {
            result = curResult;
        }
        else
        {
            settingsMap.insert(std::make_pair(std::move(field), std::move(value)));
        }
    }

    settings->FillSettings(settingsMap);

    TRACE_DBG << TRACE_HEADER << settings;

    return result;
}

}  // namespace settings_provider
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::ISettingsProvider, srv::settings_provider::SettingsProvider);
