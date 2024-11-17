#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include "task_handler.h"

namespace taskmgr
{

TaskHandler::TaskHandler(std::shared_ptr<srv::IServiceLocator> locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_locator(std::move(locator))
{
    TRACE_INF << TRACE_HEADER;
}

void TaskHandler::HandleTask(tasks::BaseTask& task)
{
    TRACE_INF << TRACE_HEADER << "Executing task";
    task.Execute();
}

}  // namespace taskmgr
