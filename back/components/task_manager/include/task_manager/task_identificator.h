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
    GetProductList,
    GetWarehouseList,
    GetInvoiceList,
    GetUserList,
    GetReportList,
    GetProductSize,
    GetWarehouseSize,
    GetInvoiceSize,
    GetUserSize,
    GetReportSize,
    CreateWarehouse);

#endif  // H_A2BBD49F_D95D_4915_A21A_9460E4360117