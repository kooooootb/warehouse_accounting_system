#ifndef H_342B7493_55C6_4665_B787_01645006966C
#define H_342B7493_55C6_4665_B787_01645006966C

#include <memory>
#include <shared_mutex>

#include <boost/asio.hpp>

#include <instrumental/types.h>
#include <tracer/tracer_provider.h>

#include <session/session_factory.h>
#include <webserver/server.h>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ws
{

class Listener : public srv::tracer::TracerProvider, public std::enable_shared_from_this<Listener>
{
public:
    Listener(const ServerSettings& settings,
        std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<asio::io_context> ioContext,
        tcp::endpoint endpoint,
        std::unique_ptr<session::ISessionFactory> sessionFactory);

    ~Listener();

    void Start(size_t tasksCount);
    void Stop(size_t tasksCount);

    void StopAll();

    void SetSettings(const ServerSettings& settings);

private:
    void DoAccept();
    void OnAccept(boost::system::error_code ec, tcp::socket socket);

private:
    std::shared_ptr<asio::io_context> m_ioContext;
    tcp::endpoint m_endpoint;
    tcp::acceptor m_acceptor;

    std::shared_mutex m_sessionFactoryMtx;
    std::unique_ptr<session::ISessionFactory> m_sessionFactory;

    std::atomic<size_t> m_tasksToStopCount;
};

}  // namespace ws

#endif  // H_342B7493_55C6_4665_B787_01645006966C