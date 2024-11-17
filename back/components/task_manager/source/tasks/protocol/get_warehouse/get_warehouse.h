#ifndef H_1B0BE985_2E18_4AE3_A72F_A0193104586D
#define H_1B0BE985_2E18_4AE3_A72F_A0193104586D

#include <string_view>

#include <instrumental/common.h>

#include <locator/service_locator.h>
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

class GetWarehouse : public BaseTask
{
public:
    GetWarehouse(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetWarehouse;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetWarehouse(srv::IAccessor& accessor);

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

private:
    Warehouse m_warehouse;
    std::vector<WarehouseItem> m_warehouseItems;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_1B0BE985_2E18_4AE3_A72F_A0193104586D