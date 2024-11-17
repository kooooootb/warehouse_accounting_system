#ifndef H_E49B77A8_2788_4753_9920_A919A40DEC08
#define H_E49B77A8_2788_4753_9920_A919A40DEC08

#include <string_view>

#include <instrumental/common.h>
#include <instrumental/types.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <document_manager/document_manager.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/invoice.h>
#include <tasks/common/operation.h>
#include <tasks/common/product.h>
#include <tasks/common/report.h>
#include <tasks/common/warehouse.h>

namespace taskmgr
{
namespace tasks
{

class ReportsCurrent : public BaseTask
{
public:
    ReportsCurrent(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::ReportsCurrent;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result CollectData(srv::IAccessor& accessor, srv::IDateProvider& dateProvider, srv::IDocumentManager& documentManager);
    ufa::Result CreateReportFile(srv::IDateProvider& dateProvider, srv::IDocumentManager& documentManager, std::ofstream& fstream);
    ufa::Result WriteReportFile(srv::IDateProvider& dateProvider, srv::IDocumentManager& documentManager, std::ofstream& fstream);

private:
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view WAREHOUSE_ID_KEY = "warehouse_id";
    static constexpr std::string_view REPORT_ID_KEY = "report_id";
    static constexpr std::string_view FILEPATH_KEY = "filepath";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view COUNT_KEY = "count";
    static constexpr std::string_view PRODUCTS_KEY = "products";
    static constexpr std::string_view MAIN_COLOR_KEY = "main_color";
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view PRETTY_NAME_KEY = "pretty_name";

private:
    Report m_report;
    Warehouse m_warehouse;
    std::map<int64_t, Product> m_products;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_E49B77A8_2788_4753_9920_A919A40DEC08