#include <filesystem>
#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <mutex>

#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <document_manager/document_manager.h>
#include <instrumental/settings.h>
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
#include "session_factory.h"

namespace ws
{
namespace session
{

constexpr bool DEFAULT_IS_SECURED = true;

namespace
{

class PlainSessionProvider : public srv::tracer::TracerProvider, public ISessionProvider
{
public:
    PlainSessionProvider(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<taskmgr::ITaskManager> taskManager)
        : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
        , m_documentManager(locator->GetInterface<srv::IDocumentManager>())
        , m_authorizer(locator->GetInterface<srv::IAuthorizer>())
        , m_taskManager(std::move(taskManager))
    {
        TRACE_INF << TRACE_HEADER;
    }

    std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) override
    {
        TRACE_INF << TRACE_HEADER;

        return std::make_shared<PlainSession>(GetTracer(), m_authorizer, m_documentManager, m_taskManager, std::move(socket));
    }

private:
    std::shared_ptr<srv::IDocumentManager> m_documentManager;
    std::shared_ptr<srv::IAuthorizer> m_authorizer;

    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
};

class SecuredSessionProvider : public srv::tracer::TracerProvider, public ISessionProvider
{
public:
    SecuredSessionProvider(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<taskmgr::ITaskManager> taskManager)
        : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
        , m_documentManager(locator->GetInterface<srv::IDocumentManager>())
        , m_authorizer(locator->GetInterface<srv::IAuthorizer>())
        , m_taskManager(std::move(taskManager))
        , m_sslContext(ssl::context::tlsv12)
    {
        TRACE_INF << TRACE_HEADER;

        LoadServerCertificate(m_sslContext);
    }

    std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) override
    {
        TRACE_INF << TRACE_HEADER;

        return std::make_shared<SecuredSession>(GetTracer(),
            m_authorizer,
            m_documentManager,
            m_taskManager,
            std::move(socket),
            m_sslContext);
    }

private:
    std::shared_ptr<srv::IDocumentManager> m_documentManager;
    std::shared_ptr<srv::IAuthorizer> m_authorizer;

    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;

    ssl::context m_sslContext;
};

}  // namespace

SessionFactory::SessionFactory(const ServerSettings& settings,
    std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<taskmgr::ITaskManager> taskManager)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_savedIsSecured(DEFAULT_IS_SECURED)
    , m_locator(std::move(locator))
    , m_taskManager(std::move(taskManager))
{
    TRACE_INF << TRACE_HEADER;
}

void SessionFactory::SetSettings(const ServerSettings& settings)
{
    std::lock_guard lock(m_settingsMutex);

    ufa::TryExtractFromOptional(settings.isSecured, m_savedIsSecured);

    if (m_savedIsSecured)
    {
        m_sessionProvider = std::make_unique<SecuredSessionProvider>(m_locator, m_taskManager);
    }
    else
    {
        m_sessionProvider = std::make_unique<PlainSessionProvider>(m_locator, m_taskManager);
    }
}

std::shared_ptr<ISession> SessionFactory::CreateSession(tcp::socket&& socket)
{
    TRACE_INF << TRACE_HEADER;

    std::lock_guard lock(m_settingsMutex);

    return m_sessionProvider->CreateSession(std::move(socket));
}

std::unique_ptr<ISessionFactory> ISessionFactory::Create(const ServerSettings& settings,
    std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<taskmgr::ITaskManager> taskManager)
{
    return std::make_unique<SessionFactory>(settings, std::move(locator), std::move(taskManager));
}

}  // namespace session
}  // namespace ws
