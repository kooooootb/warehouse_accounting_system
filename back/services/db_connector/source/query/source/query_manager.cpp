#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <tracer/tracer_provider.h>

#include <db_connector/query/query_options.h>

#include "query_manager.h"

namespace srv
{
namespace db
{
namespace qry
{

QueryManager::QueryManager(const DBConnectorSettings& settings, const std::shared_ptr<srv::IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
    TRACE_INF << TRACE_HEADER;
}

std::unique_ptr<IQueryManager> IQueryManager::Create(const DBConnectorSettings& settings,
    const std::shared_ptr<IServiceLocator>& locator)
{
    return std::make_unique<QueryManager>(settings, locator);
}

IQueryManager::QueriesLock QueryManager::GetQueries()
{
    return IQueryManager::QueriesLock(m_queriesMutex, m_supportesQueries);
}

uint64_t QueryManager::GetOrSupportQueryId(std::unique_ptr<IQueryOptions>&& queryOptions)
{
    std::lock_guard lock(m_queriesMutex);
    const auto willUseId = m_currentId++;
    m_supportesQueries.emplace(willUseId, std::move(queryOptions));
    return willUseId;
}

}  // namespace qry
}  // namespace db
}  // namespace srv
