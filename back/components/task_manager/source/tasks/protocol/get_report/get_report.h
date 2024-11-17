#ifndef H_27F9051F_9D15_4E4D_B354_71914D1C860D
#define H_27F9051F_9D15_4E4D_B354_71914D1C860D

#include <string_view>

#include <instrumental/common.h>

#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/report.h>

namespace taskmgr
{
namespace tasks
{

class GetReport : public BaseTask
{
public:
    GetReport(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetReport;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetReport(srv::IAccessor& accessor);

private:
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view REPORT_ID_KEY = "report_id";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view REPORT_TYPE_KEY = "report_type";
    static constexpr std::string_view FILEPATH_KEY = "filepath";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view WAREHOUSE_ID_KEY = "warehouse_id";

private:
    Report m_report;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_27F9051F_9D15_4E4D_B354_71914D1C860D