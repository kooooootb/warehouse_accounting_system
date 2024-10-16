#include <filesystem>
#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <document_manager/document_manager.h>
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

namespace
{

class PlainSessionFactory : public srv::tracer::TracerProvider, public ISessionFactory
{
public:
    PlainSessionFactory(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<taskmgr::ITaskManager> taskManager)
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

        return std::make_shared<PlainSession>(GetTracer(), m_taskManager, m_authorizer, m_documentManager, std::move(socket));
    }

private:
    std::shared_ptr<srv::IDocumentManager> m_documentManager;
    std::shared_ptr<srv::IAuthorizer> m_authorizer;

    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
};

class SecuredSessionFactory : public srv::tracer::TracerProvider, public ISessionFactory
{
public:
    SecuredSessionFactory(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<taskmgr::ITaskManager> taskManager)
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
            m_taskManager,
            m_authorizer,
            m_documentManager,
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

std::unique_ptr<ISessionFactory> Create(const ServerSettings& settings,
    std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<taskmgr::ITaskManager> taskManager)
{
    if (settings.isSecured)
        return std::make_unique<SecuredSessionFactory>(std::move(locator), std::move(taskManager));
    else
        return std::make_unique<PlainSessionFactory>(std::move(locator), std::move(taskManager));
}

}  // namespace session
}  // namespace ws
