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

std::pair<std::string, uint32_t> SelectQuery::SerializeParametrized()
{
    using namespace std::literals;

    TRACE_INF << TRACE_HEADER;

    std::string result;
    pqxx::placeholders placeholders;

    result += fmt::format("SELECT {} FROM {} "sv,
        string_converters::ToString(std::begin(m_options->columns), std::end(m_options->columns), ", "sv),
        string_converters::ToString(m_options->table));

    for (const auto& join : m_options->joins)
    {
        result += fmt::format(" {} JOIN {} ON {}.{}={}.{} "sv,
            string_converters::ToString(join.type),
            string_converters::ToString(join.joiningTable),
            string_converters::ToString(join.leftTable),
            string_converters::ToString(join.leftColumn),
            string_converters::ToString(join.joiningTable),
            string_converters::ToString(join.joiningColumn));
    }

    if (m_options->condition != nullptr)
    {
        result += fmt::format("WHERE {} "sv, m_options->condition->ToString(placeholders));
    }

    result += ';';

    TRACE_DBG << TRACE_HEADER << "Serialized select query: " << result;

    return {result, placeholders.count() - 1};  // count will return index of new placeholder
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
