#include <tracer/tracer_provider.h>

#include <db_connector/query/query_factory.h>

#include "delete_query.h"

namespace srv
{
namespace db
{

DeleteQuery::DeleteQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<DeleteOptions>&& options, DeleteValues&& values)
    : BaseQuery(std::move(tracer), std::move(options))
{
    TRACE_INF << TRACE_HEADER;

    CHECK_TRUE(options->condition != nullptr, "Empty condition in DELETE is prohibited");

    ExtractParamsFromCondition();
}

template <>
std::unique_ptr<IQuery> QueryFactory::Create(std::shared_ptr<srv::ITracer> tracer,
    std::unique_ptr<DeleteOptions>&& options,
    DeleteValues&& values)
{
    return std::make_unique<DeleteQuery>(std::move(tracer), std::move(options), std::move(values));
}

QueryIdentificator DeleteQuery::GetIdentificator()
{
    return QueryIdentificator::DELETE;
}

params_t DeleteQuery::ExtractParams()
{
    TRACE_INF << TRACE_HEADER << "Extracing values";

    return std::move(m_conditionParams);
}

void DeleteQuery::ExtractParamsFromCondition()
{
    if (m_options->condition != nullptr)
    {
        m_options->condition->CollectParams(m_conditionParams);
    }
}

}  // namespace db
}  // namespace srv
