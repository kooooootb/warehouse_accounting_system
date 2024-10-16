#ifndef H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198
#define H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <task_manager/task_manager.h>
#include <webserver/server.h>

#include "listener.h"
#include "worker.h"

namespace asio = boost::asio;  // dumb clangd cant find asio ns w/out using

namespace ws
{

class Server : public IServer, public srv::tracer::TracerProvider
{
public:
    Server(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<taskmgr::ITaskManager> taskManager);

    ~Server() noexcept override;

    ufa::Result Start() override;
    ufa::Result Stop() override;
    void SetSettings(const ServerSettings& settings) override;

private:
    void SetWorkers(int numWorkers);
    void RunWorker();
    void RemoveJoinedWorkers();

private:
    std::shared_ptr<asio::io_context> m_ioContext;

    std::mutex m_workersMutex;  // for changing workers count
    size_t m_workersCount;      // m_workers.size() can be bigger cause worker didnt stop after reducing them
    std::vector<std::unique_ptr<Worker>> m_workers;
    asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;

    std::unique_ptr<Listener> m_listener;

    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
};

}  // namespace ws

#endif  // H_F0D260A1_91BD_4ACD_A03B_CF935FB0E198