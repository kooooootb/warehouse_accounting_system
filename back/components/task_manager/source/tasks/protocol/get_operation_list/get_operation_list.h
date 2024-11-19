#ifndef H_11C5EA21_1C89_4891_A46A_53C75424E259
#define H_11C5EA21_1C89_4891_A46A_53C75424E259

#include <string_view>

#include <instrumental/common.h>

#include <db_connector/query/condition.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/operation.h>

namespace taskmgr
{
namespace tasks
{

class GetOperationList : public BaseTask
{
public:
    GetOperationList(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetOperationList;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetOperationList(srv::IAccessor& accessor);

private:
    static constexpr std::string_view OPERATION_ID_KEY = "operation_id";
    static constexpr std::string_view INVOICE_ID_KEY = "invoice_id";
    static constexpr std::string_view INVOICE_PRETTY_NAME_KEY = "invoice_pretty_name";
    static constexpr std::string_view PRODUCT_ID_KEY = "product_id";
    static constexpr std::string_view WAREHOUSE_FROM_ID_KEY = "warehouse_from_id";
    static constexpr std::string_view WAREHOUSE_TO_ID_KEY = "warehouse_to_id";
    static constexpr std::string_view COUNT_KEY = "count";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view RESULT_KEY = "result";
    static constexpr std::string_view LIMIT_KEY = "limit";
    static constexpr std::string_view OFFSET_KEY = "offset";
    static constexpr std::string_view FILTERS_KEY = "filters";

private:
    std::vector<Operation> m_operations;
    int64_t m_limit;
    int64_t m_offset;
    std::unique_ptr<srv::db::ICondition> m_filter;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_11C5EA21_1C89_4891_A46A_53C75424E259