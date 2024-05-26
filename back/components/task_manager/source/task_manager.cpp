#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include "task_manager.h"

namespace taskmgr
{

TaskManager::TaskManager(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor,
    std::shared_ptr<auth::IAuthorizer> authorizer)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
}

std::unique_ptr<ITaskManager> ITaskManager::Create(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor,
    std::shared_ptr<auth::IAuthorizer> authorizer)
{
    return std::make_unique<TaskManager>(std::move(locator), std::move(accessor), std::move(authorizer));
}

TaskManager::~TaskManager() noexcept {}

void TaskManager::AddTask(std::unique_ptr<ITask>&& task) {}

}  // namespace taskmgr
