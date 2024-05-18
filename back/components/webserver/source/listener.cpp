#include <boost/beast.hpp>

#include <instrumental/types.h>
#include <tracer/tracer_provider.h>

#include "listener.h"
#include "server.h"

namespace beast = boost::beast;

namespace ws
{

Listener::Listener(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<asio::io_context> ioContext,
    tcp::endpoint endpoint,
    std::unique_ptr<ISessionFactory> sessionFactory)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_ioContext(ioContext)
    , m_acceptor(*ioContext)
    , m_sessionFactory(std::move(sessionFactory))
{
    TRACE_INF << TRACE_HEADER << "Creating listener";

    boost::system::error_code ec;

    m_acceptor.open(endpoint.protocol(), ec);
    CHECK_TRUE(!ec, "Open acceptor failed with error: " << ec << ", message: " << ec.message());

    m_acceptor.set_option(asio::socket_base::reuse_address(true), ec);
    CHECK_TRUE(!ec, "Set option failed with error: " << ec << ", message: " << ec.message());

    m_acceptor.bind(endpoint, ec);
    CHECK_TRUE(!ec, "Binding acceptor failed with error: " << ec << ", message: " << ec.message());

    m_acceptor.listen(asio::socket_base::max_listen_connections, ec);
    CHECK_TRUE(!ec, "Listening failed with error: " << ec << ", message: " << ec.message());
}

int Listener::GetPort() const
{
    return m_endpoint.port();
}

asio::ip::address Listener::GetAddress() const
{
    return m_endpoint.address();
}

void Listener::Start()
{
    TRACE_INF << TRACE_HEADER << "Starting listener";

    DoAccept();
}

void Listener::Stop()
{
    TRACE_INF << TRACE_HEADER << "Closing listener";

    boost::system::error_code ec;
    m_acceptor.close(ec);
    CHECK_TRUE(!ec, "Acceptor close failed with error: " << ec << ", message: " << ec.message());
}

void Listener::DoAccept()
{
    m_acceptor.async_accept(asio::make_strand(*m_ioContext), beast::bind_front_handler(&Listener::OnAccept, shared_from_this()));
}

void Listener::OnAccept(boost::system::error_code ec, tcp::socket socket)
{
    // Check error, exception will prevent futher accepting
    CHECK_TRUE(!ec, "async_accept failed with error: " << ec << ", message: " << ec.message());

    // Create the session ad run it
    std::shared_ptr<ISession> session;
    {
        std::shared_lock lock(m_sessionFactoryMtx);
        session = m_sessionFactory->CreateSession(std::move(socket));
    }
    session->Run();

    // Loop accepting
    DoAccept();
}

void Listener::SetSessionFactory(std::unique_ptr<ISessionFactory> sessionFactory)
{
    std::unique_lock lock(m_sessionFactoryMtx);
    m_sessionFactory = std::move(sessionFactory);
}

}  // namespace ws