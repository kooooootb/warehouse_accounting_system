#ifndef H_6EA1E626_56DC_4E42_AFE2_254DCA4752F0
#define H_6EA1E626_56DC_4E42_AFE2_254DCA4752F0

#include <db_connector/query/delete_query_params.h>
#include <db_connector/query/query.h>

#include "base_query.h"

namespace srv
{
namespace db
{

class DeleteQuery : public BaseQuery<DeleteOptions>
{
public:
    DeleteQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<DeleteOptions>&& options, DeleteValues&& values);

    QueryIdentificator GetIdentificator() override;

    params_t ExtractParams() override;

private:
    void ExtractParamsFromCondition();

private:
    params_t m_conditionParams;
};

}  // namespace db
}  // namespace srv

#endif  // H_6EA1E626_56DC_4E42_AFE2_254DCA4752F0