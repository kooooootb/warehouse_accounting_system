#ifndef H_753B3985_6E73_456C_BC2F_4415FE1A8B3C
#define H_753B3985_6E73_456C_BC2F_4415FE1A8B3C

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include <instrumental/common.h>

#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <tasks/base_task.h>

#include "task_handler.h"

namespace taskmgr
{

constexpr int DEFAULT_NUM_WORKERS = 4;

class WorkersManager : public srv::tracer::TracerProvider
{
public:
    WorkersManager(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<TaskHandler> taskHandler);

    ~WorkersManager() noexcept;

    void AddTask(std::unique_ptr<tasks::BaseTask>&& task);

private:
    void Run();

private:
    std::shared_ptr<TaskHandler> m_taskHandler;

    std::vector<std::thread> m_workers;

    std::queue<std::unique_ptr<tasks::BaseTask>> m_queue;
    std::mutex m_queueMtx;
    std::condition_variable m_queueCv;
    std::atomic<bool> m_stop{false};
};

}  // namespace taskmgr

#endif  // H_753B3985_6E73_456C_BC2F_4415FE1A8B3C