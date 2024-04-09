#include "task_handler.h"
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

namespace taskmgr
{

TaskHandler::TaskHandler(std::shared_ptr<srv::IServiceLocator> locator, std::unique_ptr<deps::IDependencyManager> depManager)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_depManager(std::move(depManager))
{
}

void TaskHandler::HandleTask(tasks::BaseTask& task)
{
    TRACE_INF << TRACE_HEADER << "Executing task";
    task.Execute(*m_depManager);
}

}  // namespace taskmgr
