#ifndef H_2FB6A20C_2E07_4269_919F_4D918ECA4549
#define H_2FB6A20C_2E07_4269_919F_4D918ECA4549

#include <string_view>

#include <instrumental/common.h>

#include <db_connector/query/condition.h>
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

class GetReportList : public BaseTask
{
public:
    GetReportList(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetReportList;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetReportList(srv::IAccessor& accessor);

private:
    static constexpr std::string_view REPORT_ID_KEY = "report_id";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view REPORT_TYPE_KEY = "report_type";
    static constexpr std::string_view FILEPATH_KEY = "filepath";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view WAREHOUSE_ID_KEY = "warehouse_id";
    static constexpr std::string_view RESULT_KEY = "result";
    static constexpr std::string_view LIMIT_KEY = "limit";
    static constexpr std::string_view OFFSET_KEY = "offset";
    static constexpr std::string_view FILTERS_KEY = "filters";

private:
    std::vector<Report> m_reports;
    int64_t m_limit;
    int64_t m_offset;
    std::unique_ptr<srv::db::ICondition> m_filter;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_2FB6A20C_2E07_4269_919F_4D918ECA4549