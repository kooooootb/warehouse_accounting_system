#ifndef H_C1DA314E_7670_407C_B283_13C043B194F7
#define H_C1DA314E_7670_407C_B283_13C043B194F7

#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tasks/base_task.h>
#include <tracer/tracer_provider.h>

namespace taskmgr
{

class TaskHandler : public srv::tracer::TracerProvider
{
public:
    TaskHandler(std::shared_ptr<srv::IServiceLocator> locator);

    void HandleTask(tasks::BaseTask& task);

private:
    std::shared_ptr<srv::IServiceLocator> m_locator;
};

}  // namespace taskmgr

#endif  // H_C1DA314E_7670_407C_B283_13C043B194F7
