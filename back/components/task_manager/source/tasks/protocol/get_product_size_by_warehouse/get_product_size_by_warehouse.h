#ifndef H_0DA08383_2127_46FE_9981_1826562A8665
#define H_0DA08383_2127_46FE_9981_1826562A8665

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

class GetProductSizeByWarehouse : public BaseTask
{
public:
    GetProductSizeByWarehouse(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<srv::IServiceLocator> locator,
        const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetProductSizeByWarehouse;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetProductSizeByWarehouse(srv::IAccessor& accessor, int32_t& count);

private:
    static constexpr std::string_view WAREHOUSE_ID_KEY = "warehouse_id";
    static constexpr std::string_view COUNT_KEY = "count";

private:
    int64_t m_warehouse_id;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_0DA08383_2127_46FE_9981_1826562A8665