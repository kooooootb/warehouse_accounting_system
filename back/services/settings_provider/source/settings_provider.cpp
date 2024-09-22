#include <environment/environment.h>
#include <instrumental/common.h>
#include <instrumental/types.h>

#include <algorithm>
#include <cctype>
#include <filesystem>

#include "config_reader.h"
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

SettingsProvider::SettingsProvider(IServiceLocator* locator)
{
    std::filesystem::path configPath;

    if (locator->GetInterface(m_environment) == ufa::Result::SUCCESS)
    {
        std::string configPathStr;
        if (m_environment->GetValue(SETTINGS_FILE_ENV, configPathStr) == ufa::Result::SUCCESS)
        {
            configPath = std::move(configPathStr);
        }
    }

    if (configPath.empty())
    {
        configPath = std::filesystem::current_path() / SETTINGS_FILENAME;
    }

    m_configReader = std::make_unique<ConfigReader>(std::move(configPath));
}

bool SettingsProvider::TryFromEnvironment(std::string_view settingsName, std::string_view name, std::string& value) const
{
    if (m_environment != nullptr)
    {
        if (m_environment->GetValue(ConvertToEnvironmentKey(settingsName, name), value) == ufa::Result::SUCCESS)
        {
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
            curResult = m_configReader->ReadValue(field, value);
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

    return result;
}

}  // namespace settings_provider
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::ISettingsProvider, srv::settings_provider::SettingsProvider);
