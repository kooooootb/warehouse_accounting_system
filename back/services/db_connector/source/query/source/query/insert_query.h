#ifndef H_8C72D244_7CF3_4479_9F4A_1294425E4C41
#define H_8C72D244_7CF3_4479_9F4A_1294425E4C41

#include <db_connector/query/insert_query_params.h>
#include <db_connector/query/query.h>
#include <db_connector/query/utilities.h>

#include "base_query.h"

namespace srv
{
namespace db
{

class InsertQuery : public BaseQuery<InsertOptions>
{
public:
    InsertQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<InsertOptions>&& options, InsertValues&& values);

    QueryIdentificator GetIdentificator() override;

    params_t ExtractParams() override;

private:
    params_t m_params;
};

}  // namespace db
}  // namespace srv

#endif  // H_8C72D244_7CF3_4479_9F4A_1294425E4C41