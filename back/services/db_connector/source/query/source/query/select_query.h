#ifndef H_A85BD73B_C4F2_4AA1_A730_4A7838ACF8C1
#define H_A85BD73B_C4F2_4AA1_A730_4A7838ACF8C1

#include <db_connector/query/query.h>
#include <db_connector/query/select_query_params.h>

#include "base_query.h"

namespace srv
{
namespace db
{

class SelectQuery : public BaseQuery<SelectOptions>
{
public:
    SelectQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<SelectOptions>&& options, SelectValues&& values);

    QueryIdentificator GetIdentificator() override;

    params_t ExtractParams() override;

private:
    void ExtractParamsFromCondition();

private:
    params_t m_conditionParams;
};

}  // namespace db
}  // namespace srv

#endif  // H_A85BD73B_C4F2_4AA1_A730_4A7838ACF8C1
