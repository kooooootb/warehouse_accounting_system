#include <instrumental/string_converters.h>

#include <tracer/tracer_provider.h>

#include "connection.h"

namespace srv
{
namespace db
{
namespace conn
{

Connection::Connection(std::shared_ptr<srv::ITracer> tracer, const ConnectionOptions& options)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_rawConnection(options.GetConnectionString())
{
    TRACE_INF << TRACE_HEADER;
}

void Connection::RefreshSupportedQueries(qry::IQueryManager& queryManager)
{
    TRACE_INF << TRACE_HEADER;

    auto queries = queryManager.GetQueries();
    auto queriesIt = queries->rbegin();

    const auto realLast = queriesIt->id;

    if (m_lastSupportedQuery < realLast)
    {
        do
        {
            SupportQuery(queriesIt->id, queriesIt->queryOptions->SerializeParametrized());
            ++queriesIt;
        } while (queriesIt->id <= m_lastSupportedQuery);
    }
}

uint64_t Connection::GetLastSupportedQueryId()
{
    TRACE_INF << TRACE_HEADER << m_lastSupportedQuery;

    return m_lastSupportedQuery;
}

pqxx::connection& Connection::GetRaw()
{
    TRACE_INF << TRACE_HEADER;

    return m_rawConnection;
}

void Connection::SupportQuery(uint64_t id, const std::string& parametrizedQuery)
{
    TRACE_INF << TRACE_HEADER;

    m_rawConnection.prepare(string_converters::ToString(id), parametrizedQuery);
}

}  // namespace conn
}  // namespace db
}  // namespace srv
