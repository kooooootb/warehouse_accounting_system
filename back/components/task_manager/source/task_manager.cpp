#include <exception>

#include <instrumental/types.h>

#include <authorizer/authorizer.h>
#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <task_manager/task_info.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include <protocol/tasks_list.h>
#include <tasks/base_task.h>

#include "task_manager.h"
#include "workers_manager.h"

namespace taskmgr
{

namespace
{

template <std::size_t index>
inline typename std::enable_if<index == std::tuple_size_v<tasks::TasksList>, ufa::Result>::type GenerateTask(
    std::shared_ptr<srv::ITracer> tracer,
    TaskInfo&& taskInfo,
    std::unique_ptr<tasks::BaseTask>& task)
{
    LOCAL_TRACER(tracer);
    TRACE_ERR << TRACE_HEADER << "Task not found, identificator: " << taskInfo.identificator;

    return ufa::Result::NOT_FOUND;
}

template <std::size_t index = 0>
inline typename std::enable_if<std::less<std::size_t>{}(index, std::tuple_size_v<tasks::TasksList>), ufa::Result>::type GenerateTask(
    std::shared_ptr<srv::ITracer> tracer,
    TaskInfo&& taskInfo,
    std::unique_ptr<tasks::BaseTask>& task)
{
    LOCAL_TRACER(tracer);

    using Task = std::tuple_element_t<index, tasks::TasksList>;
    if (taskInfo.identificator == Task::GetIdentificator())
    {
        try
        {
            task = std::make_unique<Task>(tracer, std::move(taskInfo));
            TRACE_INF << TRACE_HEADER << "Found task, identificator: " << Task::GetIdentificator();
        }
        catch (const std::exception& ex)
        {
            TRACE_ERR << TRACE_HEADER << ex.what() << ", identificator: " << Task::GetIdentificator();
            return ufa::Result::WRONG_FORMAT;
        }
    }

    return GenerateTask<index + 1>(std::move(tracer), std::move(taskInfo), task);
}

}  // namespace

TaskManager::TaskManager(std::shared_ptr<srv::IServiceLocator> locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_taskHandler(std::make_shared<TaskHandler>(locator))
    , m_workersManager(std::make_unique<WorkersManager>(locator, m_taskHandler))
{
    TRACE_INF << TRACE_HEADER;
}

std::unique_ptr<ITaskManager> ITaskManager::Create(std::shared_ptr<srv::IServiceLocator> locator)
{
    return std::make_unique<TaskManager>(std::move(locator));
}

ufa::Result TaskManager::AddTask(TaskInfo&& taskInfo)
{
    TRACE_INF << TRACE_HEADER;

    std::unique_ptr<tasks::BaseTask> task;
    const auto result = GenerateTask(GetTracer(), std::move(taskInfo), task);

    if (result == ufa::Result::SUCCESS)
    {
        m_workersManager->AddTask(std::move(task));
    }

    return result;
}

}  // namespace taskmgr
