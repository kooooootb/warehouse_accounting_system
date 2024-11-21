#include <tracer/tracer_provider.h>

#include <db_connector/query/query_factory.h>

#include "update_query.h"

namespace srv
{
namespace db
{

UpdateQuery::UpdateQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<UpdateOptions>&& options, UpdateValues&& values)
    : BaseQuery(std::move(tracer), std::move(options))
{
    TRACE_DBG << TRACE_HEADER;

    CHECK_TRUE(values.values.size() == m_options->columns.size(), "Columns count doesn't match values count");
    CHECK_TRUE(m_options->condition != nullptr, "UPDATE query without condition is prohibited");

    values.values.ExtractTo(m_params);
    m_options->condition->CollectParams(m_params);
}

template <>
std::unique_ptr<IQuery> QueryFactory::Create(std::shared_ptr<srv::ITracer> tracer,
    std::unique_ptr<UpdateOptions>&& options,
    UpdateValues&& values)
{
    return std::make_unique<UpdateQuery>(std::move(tracer), std::move(options), std::move(values));
}

QueryIdentificator UpdateQuery::GetIdentificator()
{
    return QueryIdentificator::UPDATE;
}

params_t UpdateQuery::ExtractParams()
{
    TRACE_INF << TRACE_HEADER << "Extracing values";

    return std::move(m_params);
}

}  // namespace db
}  // namespace srv
