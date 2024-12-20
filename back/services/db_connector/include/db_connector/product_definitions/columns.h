#ifndef H_7FCF8B8D_64FB_462B_9D8C_49A8D552F035
#define H_7FCF8B8D_64FB_462B_9D8C_49A8D552F035

#include <instrumental/serialized_enum.h>

DEFINE_ENUM_WITH_SERIALIZATION(srv::db,
    Column,
    Invalid,
    report_id,
    name,
    report_type,
    filepath,
    created_date,
    created_by,
    warehouse_id,
    pretty_name,
    description,
    location,
    user_id,
    login,
    password_hashed,
    count,
    main_color,
    color_value,
    invoice_id,
    warehouse_to_id,
    warehouse_from_id,
    company_name,
    operation_id,
    product_id,
    invoice_pretty_name)

#endif  // H_7FCF8B8D_64FB_462B_9D8C_49A8D552F035