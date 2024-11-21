#include <tracer/trace_macros.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include "workers_manager.h"

namespace taskmgr
{

WorkersManager::WorkersManager(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<TaskHandler> taskHandler)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_taskHandler(std::move(taskHandler))
{
    TRACE_DBG << TRACE_HEADER;

    for (int i = 0; i < DEFAULT_NUM_WORKERS; ++i)
    {
        m_workers.emplace_back(&WorkersManager::Run, this);
    }
}

WorkersManager::~WorkersManager() noexcept
{
    TRACE_INF << TRACE_HEADER << "Stopping";

    m_stop = true;
    m_queueCv.notify_all();

    for (auto& worker : m_workers)
    {
        worker.join();
    }
}

void WorkersManager::AddTask(std::unique_ptr<tasks::BaseTask>&& task)
{
    TRACE_DBG << TRACE_HEADER;

    {
        std::lock_guard lock(m_queueMtx);
        m_queue.emplace(std::move(task));
    }
    m_queueCv.notify_all();
}

void WorkersManager::Run()
{
    TRACE_DBG << TRACE_HEADER;

    while (!m_stop)
    {
        std::unique_lock lock(m_queueMtx);

        m_queueCv.wait(lock,
            [this]() -> bool
            {
                return !m_queue.empty() || m_stop;
            });

        if (!m_queue.empty())
        {
            auto task = std::move(m_queue.front());
            m_queue.pop();
            lock.unlock();

            try
            {
                m_taskHandler->HandleTask(*task);
            }
            catch (const std::exception& ex)
            {
                TRACE_ERR << TRACE_HEADER << "Caught exception with message: " << ex.what();
            }
            catch (...)
            {
                TRACE_ERR << TRACE_HEADER << "Caught unkown exception";
            }
        }
    }
}

}  // namespace taskmgr