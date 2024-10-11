#ifndef H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8
#define H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8

#include <instrumental/settings.h>
#include <instrumental/types.h>
#include <locator/service.h>
#include <locator/service_locator.h>

namespace srv
{

class AccessorSettings : public ufa::settings::SettingsBase
{
public:
    SETTINGS_INIT(AccessorSettings)
    {
        SETTINGS_INIT_FIELD(dbmsAddress);
        SETTINGS_INIT_FIELD(dbmsPort);
        SETTINGS_INIT_FIELD(dbmsDbname);
        SETTINGS_INIT_FIELD(dbmsUser);
        SETTINGS_INIT_FIELD(dbmsPassword);
        SETTINGS_INIT_FIELD(connectAttempts);
        SETTINGS_INIT_FIELD(alwaysReinitialize);
    }

    SETTINGS_FIELD(dbmsAddress, std::string);
    SETTINGS_FIELD(dbmsPort, uint32_t);
    SETTINGS_FIELD(dbmsDbname, std::string);
    SETTINGS_FIELD(dbmsUser, std::string);
    SETTINGS_FIELD(dbmsPassword, std::string);
    SETTINGS_FIELD(connectAttempts, uint32_t);
    SETTINGS_FIELD(alwaysReinitialize, bool);
};

struct IAccessor : public srv::IService
{
    DECLARE_IID(0X67B00E33);

    static std::unique_ptr<IAccessor> Create(std::shared_ptr<srv::IServiceLocator> locator);
};

}  // namespace srv

#endif  // H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8