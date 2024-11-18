#ifndef H_7AC39E58_C357_4586_8EC7_835E3C8BD88E
#define H_7AC39E58_C357_4586_8EC7_835E3C8BD88E

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
#include <tasks/common/invoice_item.h>

namespace taskmgr
{
namespace tasks
{

class GetInvoice : public BaseTask
{
public:
    GetInvoice(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetInvoice;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetInvoice(srv::IAccessor& accessor);

private:
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view INVOICE_ID_KEY = "invoice_id";
    static constexpr std::string_view WAREHOUSE_TO_KEY = "warehouse_to";
    static constexpr std::string_view WAREHOUSE_FROM_KEY = "warehouse_from";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view PRETTY_NAME_KEY = "pretty_name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view ITEMS_KEY = "items";
    static constexpr std::string_view PRODUCT_ID_KEY = "product_id";
    static constexpr std::string_view COUNT_KEY = "count";
    static constexpr std::string_view RESULT_KEY = "result";

private:
    Invoice m_invoice;
    std::vector<InvoiceItem> m_invoiceItems;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_7AC39E58_C357_4586_8EC7_835E3C8BD88E