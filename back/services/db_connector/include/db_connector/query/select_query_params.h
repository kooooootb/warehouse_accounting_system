#ifndef H_62C23D0F_670E_4963_A983_B475CD0B9E87
#define H_62C23D0F_670E_4963_A983_B475CD0B9E87

#include <optional>

#include <instrumental/serialized_enum.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>

#include <db_connector/query/query_options.h>

#include "condition.h"

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
};

struct SelectOptions : public IQueryOptions
{
    QueryIdentificator GetIdentificator() override
    {
        return QueryIdentificator::SELECT;
    }

    Table table;                  // e.g. .. FROM this
    std::vector<Column> columns;  // e.g. SELECT this FROM ...
    std::vector<Join> joins;      // for all joins
    Column id;
    std::unique_ptr<ICondition> condition;  // can be grouped
};

struct SelectValues  // for extendability reasons
{
};

/*
SELECT columns FROM table;
*/

/*
SELECT columns
FROM table
INNER JOIN join.joiningTable ON leftTable.leftColumn=joiningTable.joiningColumn;
*/

/*
UPDATE table
SET columns={}
WHERE id = {};
*/

/*
INSERT INTO table (columns)
VALUES ({});
*/

/*
DELETE FROM table WHERE id={};
*/

}  // namespace db
}  // namespace srv

#endif  // H_62C23D0F_670E_4963_A983_B475CD0B9E87