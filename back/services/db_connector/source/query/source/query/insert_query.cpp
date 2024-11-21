#include <instrumental/check.h>
#include <tracer/tracer_provider.h>

#include <db_connector/query/query_factory.h>

#include "insert_query.h"

namespace srv
{
namespace db
{

InsertQuery::InsertQuery(std::shared_ptr<srv::ITracer> tracer, std::unique_ptr<InsertOptions>&& options, InsertValues&& values)
    : BaseQuery(std::move(tracer), std::move(options))
{
    TRACE_DBG << TRACE_HEADER;

    // save values count in options from values
    this->m_options->valuesCount = values.values.size();

    // check columns count equality
    CHECK_TRUE(!values.values.empty(), "Insert query's values is empty, doesn't make sense");
    auto expectedSize = this->m_options->columns.size();
    for (const auto& values : values.values)
    {
        CHECK_TRUE(values.size() == expectedSize);
    }

    for (auto& collector : values.values)
    {
        collector.ExtractTo(m_params);
    }
}

template <>
std::unique_ptr<IQuery> QueryFactory::Create(std::shared_ptr<srv::ITracer> tracer,
    std::unique_ptr<InsertOptions>&& options,
    InsertValues&& values)
{
    return std::make_unique<InsertQuery>(std::move(tracer), std::move(options), std::move(values));
}

QueryIdentificator InsertQuery::GetIdentificator()
{
    return QueryIdentificator::INSERT;
}

params_t InsertQuery::ExtractParams()
{
    TRACE_INF << TRACE_HEADER << "Extracing values";

    return std::move(m_params);
}

}  // namespace db
}  // namespace srv
