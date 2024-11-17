#ifndef H_294D454B_E4FC_4D70_A30E_74F43F7E11A5
#define H_294D454B_E4FC_4D70_A30E_74F43F7E11A5

#include <string_view>

#include <instrumental/common.h>

#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/warehouse.h>

namespace taskmgr
{
namespace tasks
{

class CreateWarehouse : public BaseTask
{
public:
    CreateWarehouse(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::CreateWarehouse;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualCreateWarehouse(srv::IAccessor& accessor, srv::IDateProvider& dateProvider);

private:
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view LOCATION_KEY = "location";
    static constexpr std::string_view WAREHOUSE_ID_KEY = "warehouse_id";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view PRETTY_NAME_KEY = "pretty_name";

private:
    Warehouse m_warehouse;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_294D454B_E4FC_4D70_A30E_74F43F7E11A5