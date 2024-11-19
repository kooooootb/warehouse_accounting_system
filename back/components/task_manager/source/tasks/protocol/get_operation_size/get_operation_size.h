#ifndef H_495FEEA7_8795_4F7E_B7DB_F5AE6FEC3BB3
#define H_495FEEA7_8795_4F7E_B7DB_F5AE6FEC3BB3

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

class GetOperationSize : public BaseTask
{
public:
    GetOperationSize(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetOperationSize;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetOperationSize(srv::IAccessor& accessor, int32_t& count);

private:
    static constexpr std::string_view FILTERS_KEY = "filters";
    static constexpr std::string_view COUNT_KEY = "count";

private:
    std::unique_ptr<srv::db::ICondition> m_filter;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_495FEEA7_8795_4F7E_B7DB_F5AE6FEC3BB3
