#include <functional>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <utility>

#include <boost/asio.hpp>

#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>

#include <locator/service_locator.h>
#include <settings_provider/settings_provider.h>
#include <tracer/tracer_provider.h>

#include <session/session_factory.h>

#include "listener.h"
#include "server.h"

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ws
{

constexpr int DEFAULT_WORKERS_NUMBER = 4;

Server::Server(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<taskmgr::ITaskManager> taskManager)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_ioContext(std::make_shared<asio::io_context>())
    , m_workGuard(boost::asio::make_work_guard(*m_ioContext))
    , m_taskManager(std::move(taskManager))
{
    TRACE_INF << TRACE_HEADER;

    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(locator->GetInterface(settingsProvider));

    ServerSettings settings;
    settingsProvider->FillSettings(settings);

    SetSettings(std::move(settings));
}

Server::~Server() noexcept
{
    TRACE_INF << TRACE_HEADER;

    Stop();

    for (auto& worker : m_workers)
    {
        worker->join();
    }
}

std::unique_ptr<IServer> IServer::Create(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<taskmgr::ITaskManager> taskManager)
{
    return std::make_unique<Server>(std::move(locator), std::move(taskManager));
}

void Server::SetSettings(const ServerSettings& settings)
{
    TRACE_INF << TRACE_HEADER << "Settings: " << settings;

    // Set workers
    if (settings.workers.has_value())
        SetWorkers(settings.workers.value());

    m_listener->SetSettings(settings);
}

ufa::Result Server::Start()
{
    TRACE_INF << TRACE_HEADER;

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
    TRACE_INF << TRACE_HEADER;

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
    std::lock_guard lock(m_workersMutex);

    TRACE_INF << TRACE_HEADER << "Changing server workers number to:" << numWorkers;

    CHECK_TRUE(m_ioContext != nullptr);

    RemoveJoinedWorkers();

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
        m_listener->Stop();
    }
}

void Server::RunWorker()
{
    TRACE_INF << TRACE_HEADER;

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

void Server::RemoveJoinedWorkers()
{
    m_workers.erase(std::remove_if(std::begin(m_workers),
                        std::end(m_workers),
                        [](const auto& worker) -> bool
                        {
                            return worker.IsFinished();
                        }),
        std::end(m_workers));
}

}  // namespace ws
