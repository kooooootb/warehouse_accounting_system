#include <environment/environment.h>
#include <instrumental/common.h>
#include <algorithm>
#include <filesystem>

#include "config_reader.h"
#include "settings_provider.h"

namespace srv
{

namespace settings_provider
{

SettingsProvider::SettingsProvider(IServiceLocator* locator)
{
    std::string configPath;
    std::shared_ptr<srv::IEnvironment> environment;
    if (!(locator->GetInterface(environment) == ufa::Result::SUCCESS && environment->GetValue(SETTINGS_DIR_ENV, configPath) == ufa::Result::SUCCESS))
    {
        configPath = std::filesystem::current_path();
    }
    configPath += SETTINGS_FILENAME;

    m_configReader = std::make_unique<ConfigReader>(std::move(configPath));
}

ufa::Result SettingsProvider::FillSettings(ufa::settings::SettingsBase* settings) const
{
    const auto fields = settings->GetFields();
    std::map<std::string, std::string> settingsMap;
    ufa::Result result = ufa::Result::SUCCESS;

    std::for_each(std::cbegin(fields), std::cend(fields),
        [&settingsMap, &result, this](const auto& field)
        {
            std::string value;
            const auto curResult = m_configReader->ReadValue(field, value);
            if (curResult != ufa::Result::SUCCESS)
            {
                result = curResult;
            }
            else
            {
                settingsMap.insert(std::make_pair(std::move(field), std::move(value)));
            }
        });

    settings->FillSettings(settingsMap);

    return result;
}

}  // namespace settings_provider
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::ISettingsProvider, srv::settings_provider::SettingsProvider);
