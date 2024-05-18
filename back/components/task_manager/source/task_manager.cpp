#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include "task_manager.h"

namespace taskmgr
{

TaskManager::TaskManager(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<db::IAccessor> accessor)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
}

ufa::Result ITaskManager::Create(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor,
    std::shared_ptr<ITaskManager>& taskManager)
{
    taskManager = std::make_shared<TaskManager>(std::move(locator), std::move(accessor));
    return ufa::Result::SUCCESS;
}

TaskManager::~TaskManager() noexcept {}

void TaskManager::AddTask(std::unique_ptr<ITask>&& task) {}

}  // namespace taskmgr
