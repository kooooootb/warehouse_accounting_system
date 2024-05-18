#ifndef H_342B7493_55C6_4665_B787_01645006966C
#define H_342B7493_55C6_4665_B787_01645006966C

#include <memory>
#include <shared_mutex>

#include <boost/asio.hpp>

#include <instrumental/types.h>
#include <tracer/tracer_provider.h>

#include "session_factory.h"

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ws
{

class Listener : public srv::tracer::TracerProvider, public std::enable_shared_from_this<Listener>
{
public:
    Listener(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<asio::io_context> ioContext,
        tcp::endpoint endpoint,
        std::unique_ptr<ISessionFactory> sessionFactory);

    int GetPort() const;
    asio::ip::address GetAddress() const;

    void Start();
    void Stop();

    void SetSessionFactory(std::unique_ptr<ISessionFactory> sessionFactory);

private:
    void DoAccept();
    void OnAccept(boost::system::error_code ec, tcp::socket socket);

private:
    std::shared_ptr<asio::io_context> m_ioContext;
    tcp::endpoint m_endpoint;
    tcp::acceptor m_acceptor;

    std::unique_ptr<ISessionFactory> m_sessionFactory;
    std::shared_mutex m_sessionFactoryMtx;
};

}  // namespace ws

#endif  // H_342B7493_55C6_4665_B787_01645006966C