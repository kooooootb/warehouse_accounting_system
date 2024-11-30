#ifndef H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77
#define H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77

#include <tuple>

#include "authorization/authorization.h"
#include "create_user/create_user.h"
#include "create_warehouse/create_warehouse.h"
#include "get_current_user/get_current_user.h"
#include "get_invoice/get_invoice.h"
#include "get_invoice_list/get_invoice_list.h"
#include "get_invoice_size/get_invoice_size.h"
#include "get_operation_list/get_operation_list.h"
#include "get_operation_size/get_operation_size.h"
#include "get_product/get_product.h"
#include "get_product_list/get_product_list.h"
#include "get_product_size/get_product_size.h"
#include "get_product_size_by_warehouse/get_product_size_by_warehouse.h"
#include "get_report/get_report.h"
#include "get_report_list/get_report_list.h"
#include "get_report_size/get_report_size.h"
#include "get_user/get_user.h"
#include "get_user_list/get_user_list.h"
#include "get_user_size/get_user_size.h"
#include "get_warehouse/get_warehouse.h"
#include "get_warehouse_list/get_warehouse_list.h"
#include "get_warehouse_size/get_warehouse_size.h"
#include "products_create/products_create.h"
#include "products_move/products_move.h"
#include "products_remove/products_remove.h"
#include "reports_by_period/reports_by_period.h"
#include "reports_current/reports_current.h"

namespace taskmgr
{
namespace tasks
{

using TasksList = std::tuple<Authorization,
    CreateUser,
    ProductsCreate,
    ProductsMove,
    ProductsRemove,
    ReportsByPeriod,
    ReportsCurrent,
    GetProduct,
    GetWarehouse,
    GetInvoice,
    GetUser,
    GetReport,
    GetCurrentUser,
    GetProductList,
    GetWarehouseList,
    GetInvoiceList,
    GetUserList,
    GetReportList,
    GetOperationList,
    GetProductSize,
    GetWarehouseSize,
    GetInvoiceSize,
    GetUserSize,
    GetReportSize,
    GetOperationSize,
    GetProductSizeByWarehouse,
    CreateWarehouse>;

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_27F7780D_D408_4BDD_A5B4_A58EA86EFE77