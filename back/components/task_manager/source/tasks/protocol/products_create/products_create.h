#ifndef H_DCAAA269_40F1_4D5C_8378_590C393DF791
#define H_DCAAA269_40F1_4D5C_8378_590C393DF791

#include <string_view>

#include <instrumental/common.h>
#include <instrumental/types.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/invoice.h>
#include <tasks/common/product.h>

namespace taskmgr
{
namespace tasks
{

class ProductsCreate : public BaseTask
{
public:
    ProductsCreate(std::shared_ptr<srv::ITracer> tracer, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::ProductsCreate;
    }

protected:
    ufa::Result ExecuteInternal(const srv::IServiceLocator& locator, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result CreateInvoiceProducts(srv::IAccessor& accessor, srv::IDateProvider& dateProvider);

private:
    static constexpr std::string_view INVOICE_ID_KEY = "invoice_id";
    static constexpr std::string_view INVOICE_NAME_KEY = "invoice_name";
    static constexpr std::string_view INVOICE_DESCRIPTION_KEY = "invoice_description";
    static constexpr std::string_view WAREHOUSE_TO_KEY = "warehouse_to";
    static constexpr std::string_view PRODUCTS_KEY = "products";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view MAIN_COLOR_KEY = "main_color";
    static constexpr std::string_view MAIN_COLOR_NAME_KEY = "main_color_name";
    static constexpr std::string_view COUNT_KEY = "count";
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";

private:
    Invoice m_invoice;
    std::vector<Product> m_products;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_DCAAA269_40F1_4D5C_8378_590C393DF791
