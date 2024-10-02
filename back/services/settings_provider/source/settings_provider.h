#ifndef H_7FC4B9AC_F700_4138_8F33_289EAC1C2A89
#define H_7FC4B9AC_F700_4138_8F33_289EAC1C2A89

#include <memory>

#include <config_reader/config_reader.h>
#include <environment/environment.h>
#include <locator/service_locator.h>
#include <settings_provider/settings_provider.h>

namespace srv
{
namespace settings_provider
{

class SettingsProvider : public ISettingsProvider
{
public:
    SettingsProvider(IServiceLocator* locator);

    ufa::Result FillSettings(ufa::settings::SettingsBase* settings) const override;

private:
    bool TryFromEnvironment(std::string_view settingsName, std::string_view name, std::string& value) const;

private:
    std::shared_ptr<srv::IEnvironment> m_environment;
    std::shared_ptr<srv::IConfigReader> m_configReader;
};

}  // namespace settings_provider
}  // namespace srv

#endif  // H_7FC4B9AC_F700_4138_8F33_289EAC1C2A89
