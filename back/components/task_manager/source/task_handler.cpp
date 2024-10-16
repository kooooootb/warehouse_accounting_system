#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include "task_handler.h"

namespace taskmgr
{

TaskHandler::TaskHandler(std::shared_ptr<srv::IServiceLocator> locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
    TRACE_INF << TRACE_HEADER;
}

void TaskHandler::HandleTask(tasks::BaseTask& task)
{
    TRACE_INF << TRACE_HEADER << "Executing task";
    task.Execute(*m_locator);
}

}  // namespace taskmgr
