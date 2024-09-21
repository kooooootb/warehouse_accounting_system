#ifndef H_9E06A2D1_38C5_4AE2_8BCD_B103A25C2C1A
#define H_9E06A2D1_38C5_4AE2_8BCD_B103A25C2C1A

#include <instrumental/settings.h>
#include <locator/service.h>

namespace srv
{

namespace settings_provider
{

constexpr char SETTINGS_DIR_ENV[] = "RMS_SETTINGS_DIR";
constexpr char SETTINGS_FILENAME[] = "rms_settings.cfg";

}  // namespace settings_provider

/**
 * @brief provide default (initial) settings using keys in settings object
 * 
 */
struct ISettingsProvider : public srv::IService
{
    DECLARE_IID(0XB2A6A4FB);

    virtual ufa::Result FillSettings(ufa::settings::SettingsBase* settings) const = 0;
};

}  // namespace srv

#endif  // H_9E06A2D1_38C5_4AE2_8BCD_B103A25C2C1A