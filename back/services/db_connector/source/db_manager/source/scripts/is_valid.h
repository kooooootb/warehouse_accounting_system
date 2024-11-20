#ifndef H_4652B7C1_328D_4E3F_8A77_DB3A834E8C10
#define H_4652B7C1_328D_4E3F_8A77_DB3A834E8C10

#include <string_view>

namespace srv
{
namespace db
{
namespace dbmgr
{
namespace scpts
{

constexpr std::string_view IsValid = R"(
SELECT
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Report') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Operation') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Invoice_Item') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Invoice') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Warehouse_Item') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Warehouse') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Product') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'Color') AND
    EXISTS (SELECT FROM pg_tables WHERE tablename = 'User') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Color"'::regclass AND attname = 'color_value') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Color"'::regclass AND attname = 'name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'invoice_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'description') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'created_date') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'created_by') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'company_name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'warehouse_to_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice"'::regclass AND attname = 'warehouse_from_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice_Item"'::regclass AND attname = 'invoice_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice_Item"'::regclass AND attname = 'product_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Invoice_Item"'::regclass AND attname = 'count') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Operation"'::regclass AND attname = 'operation_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Operation"'::regclass AND attname = 'invoice_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Operation"'::regclass AND attname = 'product_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Operation"'::regclass AND attname = 'warehouse_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Operation"'::regclass AND attname = 'count') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Operation"'::regclass AND attname = 'created_date') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Product"'::regclass AND attname = 'product_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Product"'::regclass AND attname = 'name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Product"'::regclass AND attname = 'pretty_name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Product"'::regclass AND attname = 'description') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Product"'::regclass AND attname = 'created_date') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Product"'::regclass AND attname = 'created_by') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Product"'::regclass AND attname = 'main_color') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Report"'::regclass AND attname = 'report_id' ) AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Report"'::regclass AND attname = 'name' ) AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Report"'::regclass AND attname = 'report_type' ) AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Report"'::regclass AND attname = 'filepath' ) AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Report"'::regclass AND attname = 'created_date' ) AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Report"'::regclass AND attname = 'warehouse_id' ) AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."User"'::regclass AND attname = 'user_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."User"'::regclass AND attname = 'login') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."User"'::regclass AND attname = 'password_hashed') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."User"'::regclass AND attname = 'name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."User"'::regclass AND attname = 'created_date') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."User"'::regclass AND attname = 'created_by') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse"'::regclass AND attname = 'warehouse_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse"'::regclass AND attname = 'name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse"'::regclass AND attname = 'pretty_name') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse"'::regclass AND attname = 'description') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse"'::regclass AND attname = 'location') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse"'::regclass AND attname = 'created_date') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse"'::regclass AND attname = 'created_by') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse_Item"'::regclass AND attname = 'warehouse_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse_Item"'::regclass AND attname = 'product_id') AND
    EXISTS (SELECT FROM pg_attribute WHERE attrelid = 'public."Warehouse_Item"'::regclass AND attname = 'count') AS result;
)";

}
}  // namespace dbmgr
}  // namespace db
}  // namespace srv

#endif  // H_4652B7C1_328D_4E3F_8A77_DB3A834E8C10