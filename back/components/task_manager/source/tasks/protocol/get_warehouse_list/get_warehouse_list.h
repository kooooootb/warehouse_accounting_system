#ifndef H_75C3AF7F_B56A_4F9B_B5EB_B64430C39AB1
#define H_75C3AF7F_B56A_4F9B_B5EB_B64430C39AB1

#include <string_view>

#include <instrumental/common.h>

#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/warehouse.h>
#include <tasks/common/warehouse_item.h>

namespace taskmgr
{
namespace tasks
{

class GetWarehouseList : public BaseTask
{
public:
    GetWarehouseList(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetWarehouseList;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetWarehouseList(srv::IAccessor& accessor);

private:
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view WAREHOUSE_ID_KEY = "warehouse_id";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view PRETTY_NAME_KEY = "pretty_name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view ITEMS_KEY = "items";
    static constexpr std::string_view PRODUCT_ID_KEY = "product_id";
    static constexpr std::string_view COUNT_KEY = "count";
    static constexpr std::string_view RESULT_KEY = "result";
    static constexpr std::string_view LIMIT_KEY = "limit";
    static constexpr std::string_view OFFSET_KEY = "offset";

private:
    std::vector<Warehouse> m_warehouses;
    std::map<int64_t, std::vector<WarehouseItem>> m_warehouseItems;
    int64_t m_limit;
    int64_t m_offset;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_75C3AF7F_B56A_4F9B_B5EB_B64430C39AB1