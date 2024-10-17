#ifndef H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198
#define H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <task_manager/task_manager.h>
#include <webserver/server.h>

#include "listener.h"

namespace asio = boost::asio;  // dumb clangd cant find asio ns w/out this

namespace ws
{

class Server : public IServer, public srv::tracer::TracerProvider
{
public:
    Server(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<taskmgr::ITaskManager> taskManager);

    ~Server() noexcept override;

    ufa::Result Start() override;
    ufa::Result Stop() override;

    /**
     * @brief set settings
     * @warning Reducing workers count in current realization will cause hanging all accepted connection
     * @todo fix warning
     */
    void SetSettings(const ServerSettings& settings) override;

private:
    void SetWorkers(int numWorkers);  // is called under settings mutex
    void RunWorker();

private:
    std::shared_ptr<asio::io_context> m_ioContext;

    std::mutex m_settingsMutex;
    std::vector<std::unique_ptr<std::thread>> m_workers;
    asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;

    std::unique_ptr<Listener> m_listener;
};

}  // namespace ws

#endif  // H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198