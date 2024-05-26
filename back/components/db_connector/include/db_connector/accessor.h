#ifndef H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8
#define H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8

#include <db_connector/data/user.h>
#include <instrumental/interface.h>
#include <instrumental/settings.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>

namespace db
{

constexpr std::string_view DEFAULT_DBMS_ADDRESS = "127.0.0.1";
constexpr int DEFAULT_DBMS_PORT = 5432;
constexpr std::string_view DEFAULT_DBMS_USER = "postgres";
constexpr std::string_view DEFAULT_DBMS_DBNAME = "rms";

class AccessorSettings : public ufa::settings::SettingsBase
{
public:
    SETTINGS_INIT(AccessorSettings)
    {
        SETTINGS_INIT_FIELD(dbmsAddress);
        SETTINGS_INIT_FIELD(dbmsPort);
        SETTINGS_INIT_FIELD(dbmsUser);
        SETTINGS_INIT_FIELD(dbmsDbname);
    }

    SETTINGS_FIELD(dbmsAddress, std::string);
    SETTINGS_FIELD(dbmsPort, uint32_t);
    SETTINGS_FIELD(dbmsUser, std::string);
    SETTINGS_FIELD(dbmsDbname, std::string);
};

struct IAccessor : public ufa::IBase
{
    virtual ufa::Result FillUser(data::User& user) = 0;

    static std::unique_ptr<IAccessor> Create(std::shared_ptr<srv::IServiceLocator> locator);
};

}  // namespace db

#endif  // H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8