#ifndef H_3BA6E402_6222_403C_BA21_A65106617701
#define H_3BA6E402_6222_403C_BA21_A65106617701

#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <document_manager/document_manager.h>
#include <instrumental/interface.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include <task_manager/task_manager.h>
#include <webserver/server.h>

#include <session/session.h>
#include <session/session_factory.h>

#include "plain_session.h"
#include "secured_session.h"
#include "server_certificate.h"

namespace ws
{
namespace session
{

struct ISessionProvider : public ufa::IBase
{
    virtual std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) = 0;
};

/**
 * @brief was made to save isSecured options between setting settings
 * factory creates provider creates session
 */
class SessionFactory : public srv::tracer::TracerProvider, public ISessionFactory
{
public:
    SessionFactory(const ServerSettings& settings,
        std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<taskmgr::ITaskManager> taskManager);

    void SetSettings(const ServerSettings& settings) override;
    std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) override;

private:
    std::mutex m_settingsMutex;
    bool m_savedIsSecured;
    std::unique_ptr<ISessionProvider> m_sessionProvider;

    std::shared_ptr<srv::IServiceLocator> m_locator;
    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
};

}  // namespace session
}  // namespace ws

#endif  // H_3BA6E402_6222_403C_BA21_A65106617701