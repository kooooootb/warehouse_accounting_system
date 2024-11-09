#include <tracer/tracer_provider.h>

#include <db_connector/query/query_factory.h>

#include "select_query.h"

namespace srv
{
namespace db
{

SelectQuery::SelectQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<SelectOptions>&& options, SelectValues&& values)
    : BaseQuery(std::move(tracer), std::move(options))
{
    TRACE_INF << TRACE_HEADER;

    ExtractParamsFromCondition();
}

template <>
std::unique_ptr<IQuery> QueryFactory::Create(std::shared_ptr<srv::ITracer> tracer,
    std::unique_ptr<SelectOptions>&& options,
    SelectValues&& values)
{
    return std::make_unique<SelectQuery>(std::move(tracer), std::move(options), std::move(values));
}

QueryIdentificator SelectQuery::GetIdentificator()
{
    return QueryIdentificator::SELECT;
}

params_t SelectQuery::ExtractParams()
{
    TRACE_INF << TRACE_HEADER << "Extracing values";

    return std::move(m_conditionParams);
}

void SelectQuery::ExtractParamsFromCondition()
{
    if (m_options->condition != nullptr)
    {
        m_options->condition->CollectParams(m_conditionParams);
    }
}

}  // namespace db
}  // namespace srv
