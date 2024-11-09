#include <instrumental/string_converters.h>
#include <instrumental/types.h>

#include <tracer/tracer_provider.h>

#include <db_connector/query/query_options.h>

#include "queries_lock.h"
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

void QueryManager::SetSettings(const DBConnectorSettings& settings)
{
    TRACE_INF << TRACE_HEADER;
}

std::unique_ptr<IQueryManager::IQueriesLock> QueryManager::GetQueries()
{
    return std::make_unique<QueriesLock>(m_queriesMutex, m_supportedQueries);
}

uint64_t QueryManager::GetOrSupportQueryId(std::unique_ptr<IQuery>&& query)
{
    const auto identificator = query->GetIdentificator();
    auto options = query->ExtractOptions();

    TRACE_INF << TRACE_HEADER << "type: " << identificator;
    queryid_t willUseId;

    {
        std::lock_guard lock(m_queriesMutex);

        const auto mapIt = m_supportedQueries.find(identificator);

        if (mapIt != m_supportedQueries.end())
        {
            auto& vec = mapIt->second;

            if (const auto vecIt = std::find_if(std::cbegin(vec),
                    std::cend(vec),
                    [&options](const auto& pair)
                    {
                        return pair.second->Equals(*options);
                    });
                vecIt != std::cend(vec))
            {
                // query already supported
                willUseId = vecIt->first;

                TRACE_INF << TRACE_HEADER << "Query already supported, id: " << willUseId;
            }
            else
            {
                // add query to vector
                willUseId = m_currentId++;

                TRACE_INF << TRACE_HEADER << "Add support for known query type " << identificator << ", id: " << willUseId
                          << ", query: " << options->SerializeParametrized();

                vec.emplace_back(willUseId, std::move(options));
            }
        }
        else
        {
            // add new query type to map and add query to new vector
            willUseId = m_currentId++;

            TRACE_INF << TRACE_HEADER << "Add new query type: " << identificator << ", and new id: " << willUseId
                      << ", query: " << options->SerializeParametrized();

            decltype(m_supportedQueries)::mapped_type vec;
            vec.emplace_back(willUseId, std::move(options));

            m_supportedQueries.insert(std::make_pair(identificator, std::move(vec)));
        }
    }

    return willUseId;
}

}  // namespace qry
}  // namespace db
}  // namespace srv
