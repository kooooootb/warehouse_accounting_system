#ifndef H_528C9648_F7EE_46C5_85B7_E6C04E6A4D6E
#define H_528C9648_F7EE_46C5_85B7_E6C04E6A4D6E

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer_provider.h>

#include "task_handler.h"
#include "workers_manager.h"

namespace taskmgr
{

class TaskManager : public srv::tracer::TracerProvider, public ITaskManager
{
public:
    TaskManager(std::shared_ptr<srv::IServiceLocator> locator);

    ufa::Result AddTask(TaskInfo&& taskInfo) override;

private:
    std::shared_ptr<srv::IServiceLocator> m_locator;
    std::shared_ptr<TaskHandler> m_taskHandler;
    std::unique_ptr<WorkersManager> m_workersManager;
};

}  // namespace taskmgr

#endif  // H_528C9648_F7EE_46C5_85B7_E6C04E6A4D6E