#ifndef H_D699EE27_0078_4685_B124_D94F834E6E55
#define H_D699EE27_0078_4685_B124_D94F834E6E55

#include <instrumental/serialized_enum.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>

DEFINE_ENUM_WITH_SERIALIZATION(srv::db, JoinType, LEFT, RIGHT, INNER);

namespace srv
{
namespace db
{

struct Join
{
    JoinType type = JoinType::LEFT;
    std::optional<Table> leftTable;
    Column leftColumn;
    Table joiningTable;
    Column joiningColumn;

    bool operator==(const Join& join) const
    {
        return type == join.type && leftTable == join.leftTable && leftColumn == join.leftColumn &&
               joiningTable == join.joiningTable && joiningColumn == join.joiningColumn;
    }
};

}  // namespace db
}  // namespace srv

#endif  // H_D699EE27_0078_4685_B124_D94F834E6E55