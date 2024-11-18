#ifndef H_D87FAE3B_F159_4FB8_96BF_72D419DA5788
#define H_D87FAE3B_F159_4FB8_96BF_72D419DA5788

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

class GetReportSize : public BaseTask
{
public:
    GetReportSize(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetReportSize;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetReportSize(srv::IAccessor& accessor, int32_t& count);

private:
    static constexpr std::string_view COUNT_KEY = "count";
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_D87FAE3B_F159_4FB8_96BF_72D419DA5788