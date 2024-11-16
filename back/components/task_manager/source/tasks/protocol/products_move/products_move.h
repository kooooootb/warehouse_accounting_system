#ifndef H_71865323_3C1F_4693_8E68_66BF3217FA4D
#define H_71865323_3C1F_4693_8E68_66BF3217FA4D

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

class ProductsMove : public BaseTask
{
public:
    ProductsMove(std::shared_ptr<srv::ITracer> tracer, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::ProductsMove;
    }

protected:
    ufa::Result ExecuteInternal(const srv::IServiceLocator& locator, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result MoveInvoiceProducts(srv::IAccessor& accessor, srv::IDateProvider& dateProvider);

private:
    static constexpr std::string_view INVOICE_ID_KEY = "invoice_id";
    static constexpr std::string_view INVOICE_NAME_KEY = "invoice_name";
    static constexpr std::string_view INVOICE_DESCRIPTION_KEY = "invoice_description";
    static constexpr std::string_view INVOICE_CREATED_DATE_KEY = "invoice_created_date";
    static constexpr std::string_view WAREHOUSE_TO_KEY = "warehouse_to";
    static constexpr std::string_view WAREHOUSE_FROM_KEY = "warehouse_from";
    static constexpr std::string_view PRODUCTS_KEY = "products";
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view COUNT_KEY = "count";

private:
    Invoice m_invoice;
    std::vector<Product> m_products;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_71865323_3C1F_4693_8E68_66BF3217FA4D
