#include <fmt/core.h>
#include <pqxx/pqxx>

#include <tracer/tracer_provider.h>

#include <db_connector/query/query_factory.h>

#include "select_query.h"

namespace srv
{
namespace db
{

SelectQuery::SelectQuery(std::shared_ptr<srv::ITracer> tracer,
    std::unique_ptr<SelectOptions>&& options,
    std::unique_ptr<SelectValues>&& values)
    : BaseQuery(std::move(tracer), std::move(options), std::move(values))
{
    TRACE_INF << TRACE_HEADER;

    ExtractParamsFromCondition();
}

template <>
std::unique_ptr<IQuery> QueryFactory::Create(std::shared_ptr<srv::ITracer> tracer,
    std::unique_ptr<SelectOptions>&& options,
    std::unique_ptr<SelectValues>&& values)
{
    return std::make_unique<SelectQuery>(std::move(tracer), std::move(options), std::move(values));
}

QueryIdentificator SelectQuery::GetIdentificator()
{
    return QueryIdentificator::SELECT;
}

pqxx::params SelectQuery::GetParams()
{
    TRACE_INF << TRACE_HEADER;

    return {};
}

void SelectQuery::ExtractParamsFromCondition()
{
    if (const auto& condition = m_options->condition; condition != nullptr)
        condition->CollectParams(m_conditionParams);
}

}  // namespace db
}  // namespace srv
