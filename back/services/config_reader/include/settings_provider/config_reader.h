#ifndef H_C7758BC6_C988_4C0B_8F41_4E6E0F06F910
#define H_C7758BC6_C988_4C0B_8F41_4E6E0F06F910

#include <instrumental/common.h>
#include <instrumental/settings.h>
#include <locator/service.h>

namespace srv
{
namespace config_reader
{

constexpr std::string_view CONFIG_ENV_KEY = "RMS_SETTINGS_FILE";

/**
 * @brief If settings file wasn't specified will try open this from pwd
 */
constexpr std::string_view CONFIG_DEFAULT_FILE = "rms_settings.cfg";

}  // namespace config_reader

struct IConfigReader : public srv::IService
{
    DECLARE_IID(0XB2A6A4FB);

    /**
     * @brief reads value identified with series of keys
     * @param keys path to required value
     * @return ufa::Result NOT_FOUND or SUCCESS
     */
    virtual ufa::Result ReadValue(std::vector<std::string_view> keys, std::string& value) = 0;
};

}  // namespace srv

#endif  // H_C7758BC6_C988_4C0B_8F41_4E6E0F06F910