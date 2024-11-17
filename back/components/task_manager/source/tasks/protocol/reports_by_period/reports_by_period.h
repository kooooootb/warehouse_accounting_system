#ifndef H_8330D3E3_CE0A_4B71_9C3A_11D335F9A755
#define H_8330D3E3_CE0A_4B71_9C3A_11D335F9A755

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

class ReportsByPeriod : public BaseTask
{
public:
    ReportsByPeriod(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::ReportsByPeriod;
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
    static constexpr std::string_view PERIOD_FROM_KEY = "period_from";
    static constexpr std::string_view PERIOD_TO_KEY = "period_to";
    static constexpr std::string_view WAREHOUSE_ID_KEY = "warehouse_id";
    static constexpr std::string_view REPORT_ID_KEY = "report_id";
    static constexpr std::string_view FILEPATH_KEY = "filepath";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view INVOICES_KEY = "invoices";
    static constexpr std::string_view INVOICE_ID_KEY = "invoice_id";
    static constexpr std::string_view INVOICE_PRETTY_NAME_KEY = "invoice_pretty_name";
    static constexpr std::string_view COUNT_KEY = "count";
    static constexpr std::string_view WAREHOUSE_TO_ID_KEY = "warehouse_to_id";
    static constexpr std::string_view WAREHOUSE_FROM_ID_KEY = "warehouse_from_id";
    static constexpr std::string_view PRODUCTS_KEY = "products";
    static constexpr std::string_view PRODUCT = "warehouse_from_id";
    static constexpr std::string_view MAIN_COLOR_KEY = "main_color";
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view PRETTY_NAME_KEY = "pretty_name";

private:
    Report m_report;
    Warehouse m_warehouse;
    std::vector<Invoice> m_invoices;
    std::map<int64_t, std::vector<Operation>> m_operations;  // invoice_id: Operation
    std::map<int64_t, Product> m_products;                   // product_id: Product
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_8330D3E3_CE0A_4B71_9C3A_11D335F9A755