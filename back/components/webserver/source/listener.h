#ifndef H_342B7493_55C6_4665_B787_01645006966C
#define H_342B7493_55C6_4665_B787_01645006966C

#include <memory>
#include <shared_mutex>

#include <boost/asio.hpp>

#include <instrumental/types.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer_provider.h>

#include <session/session_factory.h>
#include <webserver/server.h>

namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace ws
{

/**
 * @brief accepts connections using passed io_context workers
 */
class Listener : public srv::tracer::TracerProvider
{
public:
    Listener(const ServerSettings& settings,
        std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<asio::io_context> ioContext);

    // will stop associated io_context
    ~Listener();

    void AddTasks(size_t tasksCount);
    void ReduceTasks(size_t tasksCount);

    void SetSettings(const ServerSettings& settings);

private:
    void DoAccept();
    void OnAccept(boost::system::error_code ec, tcp::socket socket);

    void SetEndpoint(const ServerSettings& settings);  // will reinit acceptor and cancel all accepts

    tcp::endpoint ExtractEndpoint(const ServerSettings& settings);

private:
    std::shared_ptr<asio::io_context> m_ioContext;

    std::mutex m_settingsMutex;
    asio::ip::address m_savedAddress;
    int32_t m_savedPort;
    tcp::acceptor m_acceptor;

    std::unique_ptr<session::ISessionFactory> m_sessionFactory;

    std::atomic<int32_t> m_tasksToStopCount = 0;  // use signed type, dont use its assignment methods
    std::atomic<size_t> m_totalRunnedTasksCount = 0;
};

}  // namespace ws

#endif  // H_342B7493_55C6_4665_B787_01645006966C