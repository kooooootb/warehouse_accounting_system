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
{
    pqxx::connection(options.GetConnectionString());
}

void Connection::RefreshSupportedQueries(qry::IQueryManager& queryManager)
{
    auto queries = queryManager.GetQueries();
    auto queriesIt = std::prev(queries->end());

    const auto realLast = queriesIt->first;

    if (m_lastSupportedQuery < realLast)
    {
        while (queriesIt->first <= m_lastSupportedQuery)
        {
            SupportQuery(queriesIt->first, queryManager.SerializeQuery(queriesIt->second));
            queriesIt = std::prev(queriesIt);
        }
    }
}

uint64_t Connection::GetLastSupportedQueryId()
{
    return m_lastSupportedQuery;
}

pqxx::connection& Connection::GetRaw()
{
    return m_rawConnection;
}

void Connection::SupportQuery(uint64_t id, const std::string& parametrizedQuery)
{
    m_rawConnection.prepare(string_converters::ToString(id), parametrizedQuery);
}

}  // namespace conn
}  // namespace db
}  // namespace srv
