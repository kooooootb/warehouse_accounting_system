#ifndef H_BD0A8A5A_13CF_4444_A16F_302F9B0835F9
#define H_BD0A8A5A_13CF_4444_A16F_302F9B0835F9

#include <db_connector/query/query.h>
#include <db_connector/query/update_query_params.h>

#include "base_query.h"

namespace srv
{
namespace db
{

class UpdateQuery : public BaseQuery<UpdateOptions>
{
public:
    UpdateQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<UpdateOptions>&& options, UpdateValues&& values);

    QueryIdentificator GetIdentificator() override;

    params_t ExtractParams() override;

private:
    params_t m_params;
};

}  // namespace db
}  // namespace srv

#endif  // H_BD0A8A5A_13CF_4444_A16F_302F9B0835F9