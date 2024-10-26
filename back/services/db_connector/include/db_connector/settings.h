#ifndef H_A07E58B1_DC07_4A35_A6BF_770F3942FAB6
#define H_A07E58B1_DC07_4A35_A6BF_770F3942FAB6

#include <instrumental/settings.h>
#include <instrumental/types.h>

namespace srv
{
namespace db
{

class DBConnectorSettings : public ufa::settings::SettingsBase
{
public:
    SETTINGS_INIT(DBConnectorSettings)
    {
        SETTINGS_INIT_FIELD(dbmsAddress);
        SETTINGS_INIT_FIELD(dbmsPort);
        SETTINGS_INIT_FIELD(dbmsDbname);
        SETTINGS_INIT_FIELD(dbmsUser);
        SETTINGS_INIT_FIELD(dbmsPassword);
        SETTINGS_INIT_FIELD(connectAttempts);
        SETTINGS_INIT_FIELD(alwaysReinitialize);
        SETTINGS_INIT_FIELD(connectionCount);
    }

    SETTINGS_FIELD(dbmsAddress, std::string);
    SETTINGS_FIELD(dbmsPort, uint32_t);
    SETTINGS_FIELD(dbmsDbname, std::string);
    SETTINGS_FIELD(dbmsUser, std::string);
    SETTINGS_FIELD(dbmsPassword, std::string);
    SETTINGS_FIELD(connectAttempts, uint32_t);
    SETTINGS_FIELD(alwaysReinitialize, bool);
    SETTINGS_FIELD(connectionCount, uint32_t);
};

}  // namespace db
}  // namespace srv

#endif  // H_A07E58B1_DC07_4A35_A6BF_770F3942FAB6