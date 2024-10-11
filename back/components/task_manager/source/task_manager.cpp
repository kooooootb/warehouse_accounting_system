#include <instrumental/types.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include <db_connector/accessor.h>
#include <task_manager/task_manager.h>

#include <dependency_manager/dependency_manager.h>
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
    db::data::User user,
    std::string_view target,
    std::string&& json,
    Callback&& callback,
    std::unique_ptr<tasks::BaseTask>& task)
{
    return ufa::Result::NOT_FOUND;
}

template <std::size_t index = 0>
inline typename std::enable_if<std::less<std::size_t>{}(index, std::tuple_size_v<tasks::TasksList>), ufa::Result>::type GenerateTask(
    std::shared_ptr<srv::ITracer> tracer,
    db::data::User user,
    std::string_view target,
    std::string&& json,
    Callback&& callback,
    std::unique_ptr<tasks::BaseTask>& task)
{
    using Task = std::tuple_element_t<index, tasks::TasksList>;
    if (target == Task::GetTarget())
    {
        task = std::make_unique<Task>(std::move(tracer), std::move(user), std::move(callback));
        return task->Parse(std::move(json));
    }

    return GenerateTask<index + 1>(std::move(tracer), std::move(user), target, std::move(json), std::move(callback), task);
}

}  // namespace

TaskManager::TaskManager(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor,
    std::shared_ptr<auth::IAuthorizer> authorizer,
    std::shared_ptr<docmgr::IDocumentManager> documentManager)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_taskHandler(std::make_shared<TaskHandler>(locator,
          deps::IDependencyManager::Create(locator, std::move(accessor), std::move(authorizer), std::move(documentManager))))
    , m_workersManager(std::make_unique<WorkersManager>(locator, m_taskHandler))
{
}

std::unique_ptr<ITaskManager> ITaskManager::Create(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor,
    std::shared_ptr<auth::IAuthorizer> authorizer,
    std::shared_ptr<docmgr::IDocumentManager> documentManager)
{
    return std::make_unique<TaskManager>(std::move(locator), std::move(accessor), std::move(authorizer), std::move(documentManager));
}

ufa::Result TaskManager::AddTask(db::data::User user, std::string_view target, std::string&& json, Callback&& callback)
{
    std::unique_ptr<tasks::BaseTask> task;
    const auto result = GenerateTask(GetTracer(), std::move(user), target, std::move(json), std::move(callback), task);

    if (result == ufa::Result::SUCCESS)
    {
        m_workersManager->AddTask(std::move(task));
    }

    return result;
}

}  // namespace taskmgr
