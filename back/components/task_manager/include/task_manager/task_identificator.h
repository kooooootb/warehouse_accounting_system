#ifndef H_A2BBD49F_D95D_4915_A21A_9460E4360117
#define H_A2BBD49F_D95D_4915_A21A_9460E4360117

#include <instrumental/serialized_enum.h>

DEFINE_ENUM_WITH_SERIALIZATION(taskmgr,
    TaskIdentificator,
    Authorization,
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
    CreateWarehouse);

#endif  // H_A2BBD49F_D95D_4915_A21A_9460E4360117