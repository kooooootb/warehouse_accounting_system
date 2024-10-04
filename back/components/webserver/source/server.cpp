#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

#include <boost/asio.hpp>

#include <authorizer/authorizer.h>
#include <instrumental/settings_detail.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>
#include <settings_provider/settings_provider.h>
#include <tracer/tracer_provider.h>
#include <webserver/server.h>

#include "listener.h"
#include "server.h"
#include "session_factory.h"

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ws
{

Server::Server(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<taskmgr::ITaskManager> taskManager,
    std::shared_ptr<auth::IAuthorizer> authorizer,
    std::shared_ptr<docmgr::IDocumentManager> documentManager)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_ioContext(std::make_shared<asio::io_context>())
    , m_workGuard(boost::asio::make_work_guard(*m_ioContext))
    , m_documentManager(std::move(documentManager))
    , m_taskManager(std::move(taskManager))
    , m_authorizer(std::move(authorizer))
{
    TRACE_INF << TRACE_HEADER << "Creating Server";

    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(locator->GetInterface(settingsProvider));

    ServerSettings settings;
    settingsProvider->FillSettings(&settings);
    TRACE_DBG << TRACE_HEADER << "Input settings: " << settings;
    FillEmptySettings(settings);

    SetSettings(std::move(settings));
}

Server::~Server() noexcept
{
    Stop();

    for (auto& worker : m_workers)
    {
        worker->join();
    }
}

std::unique_ptr<IServer> IServer::Create(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<taskmgr::ITaskManager> taskManager,
    std::shared_ptr<auth::IAuthorizer> authorizer,
    std::shared_ptr<docmgr::IDocumentManager> documentManager)
{
    return std::make_unique<Server>(std::move(locator), std::move(taskManager), std::move(authorizer), std::move(documentManager));
}

void Server::SetSettings(ServerSettings&& settings)
{
    TRACE_INF << TRACE_HEADER << "Settings: " << settings;

    // Set workers
    if (settings.workers.has_value())
        SetWorkers(settings.workers.value());

    // Set listener
    int port = 0;
    asio::ip::address address;
    bool shouldResetListener = false;

    // port
    if (settings.port.has_value())
    {
        port = settings.port.value();
        shouldResetListener = true;
    }
    else
    {
        CHECK_TRUE(m_listener != nullptr);
        port = m_listener->GetPort();
    }

    // address
    if (settings.address.has_value())
    {
        address = asio::ip::make_address(std::move(settings.address.value()));
        shouldResetListener |= 1;
    }
    else
    {
        CHECK_TRUE(m_listener != nullptr);
        address = m_listener->GetAddress();
    }

    if (settings.isSecured.has_value())
        m_savedIsSecured = settings.isSecured.value();

    if (settings.documentRoot.has_value())
        m_documentManager->SetRoot(std::move(settings.documentRoot.value()));

    auto sessionFactory =
        ISessionFactory::CreateSessionFactory(GetTracer(), m_taskManager, m_authorizer, m_documentManager, m_savedIsSecured);

    if (shouldResetListener)
    {
        // Set new listener
        tcp::endpoint endpoint(std::move(address), port);

        try
        {
            m_listener = std::make_shared<Listener>(GetTracer(), m_ioContext, std::move(endpoint), std::move(sessionFactory));
        }
        catch (const std::runtime_error& ex)
        {
            TRACE_ERR << TRACE_HEADER << "Setting Listener failed with exception: " << ex.what();
            m_listener = nullptr;
        }
    }
    else if (settings.isSecured.has_value())
    {
        // Change session factory on current listener
        m_listener->SetSessionFactory(std::move(sessionFactory));
    }
}

ufa::Result Server::Start()
{
    try
    {
        m_listener->Start();
        m_ioContext->poll();
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Server start failed with exception, what():" << ex.what();
        std::cerr << "Server start failed with exception, what():" << ex.what() << std::endl;
        return ufa::Result::ERROR;
    }
    catch (...)
    {
        TRACE_ERR << TRACE_HEADER << "Server start failed with unknown exception";
        std::cerr << "Server start failed with unknown exception" << std::endl;
        return ufa::Result::ERROR;
    }

    return ufa::Result::SUCCESS;
}

ufa::Result Server::Stop()
{
    try
    {
        m_listener->Stop();
        m_ioContext->stop();
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Server stop failed with exception, what():" << ex.what();
        std::cerr << "Server stop failed with exception, what():" << ex.what() << std::endl;
        return ufa::Result::ERROR;
    }
    catch (...)
    {
        TRACE_ERR << TRACE_HEADER << "Server stop failed with unknown exception";
        std::cerr << "Server stop failed with unknown exception" << std::endl;
        return ufa::Result::ERROR;
    }

    return ufa::Result::SUCCESS;
}

void Server::SetWorkers(int numWorkers)
{
    CHECK_TRUE(m_ioContext != nullptr);
    TRACE_INF << TRACE_HEADER << "Changing server workers number to:" << numWorkers;

    if (numWorkers >= m_workers.size())
    {
        // enlarge
        for (auto i = 0; i < numWorkers - m_workers.size(); ++i)
        {
            m_workers.emplace_back(std::make_unique<std::thread>(std::bind(&Server::RunWorker, this)));
        }
    }
    else
    {
        // reduce by restarting
        // stop threads
        m_ioContext->stop();
        for (auto& worker : m_workers)
        {
            worker->join();
        }

        // reset stopped flag
        m_ioContext->restart();

        // reset std::thread vector
        m_workers.clear();
        for (int i = 0; i < numWorkers; ++i)
        {
            m_workers.emplace_back(std::make_unique<std::thread>(std::bind(&Server::RunWorker, this)));
        }

        // start polling
        m_ioContext->poll();
    }
}

void Server::RunWorker()
{
    while (true)
    {
        try
        {
            m_ioContext->run();
            break;
        }
        catch (const std::exception& ex)
        {
            TRACE_ERR << TRACE_HEADER << "Worker caught exception, what() :" << ex.what();
            std::cerr << "Worker caught exception, what() :" << ex.what() << std::endl;
        }
        catch (...)
        {
            TRACE_ERR << TRACE_HEADER << "Worker caught unknown exception";
            std::cerr << "Worker caught unknown exception" << std::endl;
        }
    }
}

void Server::FillEmptySettings(ServerSettings& settings)
{
    if (!settings.workers.has_value())
        settings.workers = DEFAULT_WORKERS_NUMBER;

    if (!settings.address.has_value())
        settings.address = DEFAULT_ADDRESS;

    if (!settings.port.has_value())
        settings.port = DEFAULT_PORT;

    if (!settings.isSecured.has_value())
        settings.isSecured = DEFAULT_IS_SECURED;
}

}  // namespace ws