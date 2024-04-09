#ifndef H_7FC4B9AC_F700_4138_8F33_289EAC1C2A89
#define H_7FC4B9AC_F700_4138_8F33_289EAC1C2A89

#include <memory>

#include <locator/service_locator.h>
#include <settings_provider/settings_provider.h>

#include "config_reader.h"

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
    std::unique_ptr<ConfigReader> m_configReader;
};

}  // namespace settings_provider
}  // namespace srv

#endif  // H_7FC4B9AC_F700_4138_8F33_289EAC1C2A89
