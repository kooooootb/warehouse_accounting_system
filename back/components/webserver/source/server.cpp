#include <functional>
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
{
    TRACE_INF << TRACE_HEADER;

    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(locator->GetInterface(settingsProvider));

    ServerSettings settings;
    settingsProvider->FillSettings(settings);

    m_listener = std::make_unique<Listener>(settings, std::move(locator), std::move(taskManager), m_ioContext);
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
    TRACE_INF << TRACE_HEADER;

    m_listener->SetSettings(settings);

    {
        std::lock_guard lock(m_settingsMutex);

        if (m_workers.empty() || settings.workers.has_value())
        {
            // Set workers
            SetWorkers(settings.workers.value_or(DEFAULT_WORKERS_NUMBER));
        }
    }
}

ufa::Result Server::Start()
{
    TRACE_INF << TRACE_HEADER;
    // I have a feeling this does nothing
    return ufa::Result::SUCCESS;
}

ufa::Result Server::Stop()
{
    TRACE_INF << TRACE_HEADER;

    try
    {
        m_ioContext->stop();
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Server stop failed with exception, what():" << ex.what();
        return ufa::Result::ERROR;
    }

    return ufa::Result::SUCCESS;
}

void Server::SetWorkers(int numWorkers)
{
    TRACE_INF << TRACE_HEADER << "Changing server workers number from: " << m_workers.size() << " to: " << numWorkers;

    CHECK_TRUE(m_ioContext != nullptr);

    if (numWorkers >= m_workers.size())
    {
        // enlarge
        const auto increaseBy = numWorkers - m_workers.size();
        for (auto i = 0; i < increaseBy; ++i)
        {
            m_workers.emplace_back(std::make_unique<std::thread>(std::bind(&Server::RunWorker, this)));
        }
        m_listener->AddTasks(numWorkers - m_workers.size());
    }
    else
    {
        // reduce by restarting
        m_listener->ReduceTasks(m_workers.size() - numWorkers);

        // stop threads
        m_ioContext->stop();
        for (auto& worker : m_workers)
        {
            worker->join();
        }

        // reset stopped flag
        m_ioContext->restart();

        // reinitialize std::thread vector
        m_workers.clear();
        for (int i = 0; i < numWorkers; ++i)
        {
            m_workers.emplace_back(std::make_unique<std::thread>(std::bind(&Server::RunWorker, this)));
        }
    }
}

void Server::RunWorker()
{
    while (true)
    {
        try
        {
            TRACE_INF << TRACE_HEADER;

            m_ioContext->run();
            break;
        }
        catch (const std::exception& ex)
        {
            TRACE_ERR << TRACE_HEADER << "Worker caught exception, what() :" << ex.what();
        }
        catch (...)
        {
            TRACE_ERR << TRACE_HEADER << "Worker caught unknown exception";
        }
    }
}

}  // namespace ws
