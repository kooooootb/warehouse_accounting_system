#include <boost/beast.hpp>
#include <mutex>

#include <instrumental/settings.h>
#include <instrumental/types.h>
#include <session/session_factory.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>
#include <webserver/server.h>

#include "listener.h"
#include "server.h"

namespace beast = boost::beast;

namespace ws
{

constexpr std::string_view DEFAULT_ADDRESS = "0.0.0.0";
constexpr int DEFAULT_PORT = 10000;

Listener::Listener(const ServerSettings& settings,
    std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<taskmgr::ITaskManager> taskManager,
    std::shared_ptr<asio::io_context> ioContext)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_ioContext(ioContext)
    , m_savedAddress(asio::ip::make_address(settings.address.value_or(std::string(DEFAULT_ADDRESS))))
    , m_savedPort(DEFAULT_PORT)
    , m_acceptor(*ioContext)
    , m_sessionFactory(session::ISessionFactory::Create(settings, std::move(locator), std::move(taskManager)))
{
    TRACE_INF << TRACE_HEADER;

    SetSettings(settings);
}

Listener::~Listener()
{
    try
    {
        TRACE_INF << TRACE_HEADER;

        boost::system::error_code ec;

        // this will cancel all future work preventing it from using deleted pointer
        // but already executed handlers may not be finished though probability is low
        m_acceptor.cancel(ec);
        m_ioContext->stop();

        CHECK_TRUE(!ec, "Acceptor close failed with error: " << ec << ", message: " << ec.message());
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << ex.what();
    }
}

void Listener::AddTasks(size_t tasksCount)
{
    TRACE_INF << TRACE_HEADER << "Starting " << tasksCount << " tasks";

    for (size_t i = 0; i < tasksCount; ++i)
    {
        DoAccept();
    }
}

void Listener::ReduceTasks(size_t tasksCount)
{
    TRACE_INF << TRACE_HEADER << "Planning to stop " << tasksCount << " tasks";

    m_tasksToStopCount += tasksCount;
}

void Listener::SetSettings(const ServerSettings& settings)
{
    SetEndpoint(settings);

    m_sessionFactory->SetSettings(settings);
}

void Listener::DoAccept()
{
    TRACE_INF << TRACE_HEADER;

    std::lock_guard lock(m_settingsMutex);

    m_acceptor.async_accept(asio::make_strand(*m_ioContext), beast::bind_front_handler(&Listener::OnAccept, this));
}

void Listener::OnAccept(boost::system::error_code ec, tcp::socket socket)
{
    TRACE_INF << TRACE_HEADER;

    // Check error, exception will prevent futher accepting
    CHECK_TRUE(!ec, "async_accept failed with error: " << ec << ", message: " << ec.message());

    // Create the session and run it
    std::shared_ptr<session::ISession> session = m_sessionFactory->CreateSession(std::move(socket));
    session->Run();

    // check if we were asked to stop some tasks
    if (const auto prevValue = m_tasksToStopCount.fetch_sub(1); prevValue <= 0)
    {
        // Restore counter, this forbids its assign method
        m_tasksToStopCount += 1;
        // Loop accepting
        DoAccept();
    }
}

void Listener::SetEndpoint(const ServerSettings& settings)
{
    std::lock_guard lock(m_settingsMutex);

    const auto endpoint = ExtractEndpoint(settings);

    if (m_acceptor.is_open())
    {
        m_acceptor.close();
    }

    boost::system::error_code ec;

    m_acceptor.open(endpoint.protocol(), ec);
    CHECK_TRUE(!ec, "Open acceptor failed with error: " << ec << ", message: " << ec.message());

    m_acceptor.set_option(asio::socket_base::reuse_address(true), ec);
    CHECK_TRUE(!ec, "Set option failed with error: " << ec << ", message: " << ec.message());

    m_acceptor.bind(endpoint, ec);
    CHECK_TRUE(!ec, "Binding acceptor failed with error: " << ec << ", message: " << ec.message());

    m_acceptor.listen(asio::socket_base::max_listen_connections, ec);
    CHECK_TRUE(!ec, "Listening failed with error: " << ec << ", message: " << ec.message());

    AddTasks(m_totalRunnedTasksCount.load());
}

tcp::endpoint Listener::ExtractEndpoint(const ServerSettings& settings)
{
    if (settings.address.has_value())
    {
        asio::ip::address address = asio::ip::make_address(settings.address.value());
        m_savedAddress = address;
    }

    ufa::TryExtractFromOptional(settings.port, m_savedPort);

    return tcp::endpoint(m_savedAddress, m_savedPort);
}

}  // namespace ws