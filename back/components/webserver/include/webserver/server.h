#ifndef H_48E0BD43_26F8_48F2_AAF9_73A59F7D7383
#define H_48E0BD43_26F8_48F2_AAF9_73A59F7D7383

#include <instrumental/interface.h>
#include <instrumental/settings.h>

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <utilities/document_manager.h>

namespace ws
{

constexpr int DEFAULT_WORKERS_NUMBER = 4;
constexpr std::string_view DEFAULT_ADDRESS = "0.0.0.0";
constexpr int DEFAULT_PORT = 10000;
constexpr bool DEFAULT_IS_SECURED = true;
constexpr std::string_view AUTHORIZATION_HEADER = "Authorization";

class ServerSettings : public ufa::settings::SettingsBase
{
public:
    SETTINGS_INIT(ServerSettings)
    {
        SETTINGS_INIT_FIELD(port);
        SETTINGS_INIT_FIELD(address);
        SETTINGS_INIT_FIELD(workers);
        SETTINGS_INIT_FIELD(isSecured);
        SETTINGS_INIT_FIELD(documentRoot);
    }

    SETTINGS_FIELD(port, int);
    SETTINGS_FIELD(address, std::string);
    SETTINGS_FIELD(workers, int);
    SETTINGS_FIELD(isSecured, bool);  // use TLS?
    SETTINGS_FIELD(documentRoot, std::string);
};

struct IServer : public ufa::IBase
{
    virtual ufa::Result Start() = 0;

    virtual ufa::Result Stop() = 0;

    virtual void SetSettings(ServerSettings&& settings) = 0;

    static std::unique_ptr<IServer> Create(std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<auth::IAuthorizer> authorizer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager);
};

}  // namespace ws

#endif  // H_48E0BD43_26F8_48F2_AAF9_73A59F7D7383