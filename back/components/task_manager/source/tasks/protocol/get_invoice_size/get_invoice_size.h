#ifndef H_9D35B42E_1236_4812_A900_DDB800BE11E5
#define H_9D35B42E_1236_4812_A900_DDB800BE11E5

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

class GetInvoiceSize : public BaseTask
{
public:
    GetInvoiceSize(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetInvoiceSize;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetInvoiceSize(srv::IAccessor& accessor, int32_t& count);

private:
    static constexpr std::string_view COUNT_KEY = "count";
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_9D35B42E_1236_4812_A900_DDB800BE11E5