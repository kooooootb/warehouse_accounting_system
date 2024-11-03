#ifndef H_F5D930DC_EBC1_4533_9231_E56787ECFAF9
#define H_F5D930DC_EBC1_4533_9231_E56787ECFAF9

#include <instrumental/serialized_enum.h>

DEFINE_ENUM_WITH_SERIALIZATION(srv::db,
    Table,
    Invalid,
    Color,
    Invoice_Item,
    Invoice,
    Operation,
    Product,
    Report,
    User,
    Warehouse,
    Warehouse_Item)

#endif  // H_F5D930DC_EBC1_4533_9231_E56787ECFAF9
