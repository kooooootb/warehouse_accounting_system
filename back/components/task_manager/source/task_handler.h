#ifndef H_C1DA314E_7670_407C_B283_13C043B194F7
#define H_C1DA314E_7670_407C_B283_13C043B194F7

#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tasks/base_task.h>
#include <tracer/tracer_provider.h>

#include <dependency_manager/dependency_manager.h>

namespace taskmgr
{

class TaskHandler : public srv::tracer::TracerProvider
{
public:
    TaskHandler(std::shared_ptr<srv::IServiceLocator> locator, std::unique_ptr<deps::IDependencyManager> depManager);

    void HandleTask(tasks::BaseTask& task);

private:
    std::unique_ptr<deps::IDependencyManager> m_depManager;
};

}  // namespace taskmgr

#endif  // H_C1DA314E_7670_407C_B283_13C043B194F7
