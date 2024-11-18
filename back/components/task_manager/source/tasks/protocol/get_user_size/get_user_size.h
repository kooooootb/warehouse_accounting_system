#ifndef H_077542AD_524C_4678_B611_EA487561D7AF
#define H_077542AD_524C_4678_B611_EA487561D7AF

#include <string_view>

#include <instrumental/common.h>

#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/invoice.h>

namespace taskmgr
{
namespace tasks
{

class GetUserSize : public BaseTask
{
public:
    GetUserSize(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetUserSize;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetUserSize(srv::IAccessor& accessor, int32_t& count);

private:
    static constexpr std::string_view COUNT_KEY = "count";
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_077542AD_524C_4678_B611_EA487561D7AF