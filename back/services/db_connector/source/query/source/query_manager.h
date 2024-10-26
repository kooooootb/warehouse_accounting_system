#ifndef H_553200B0_A64F_46F4_9F8D_F9853B75257B
#define H_553200B0_A64F_46F4_9F8D_F9853B75257B

#include <db_connector/query/query_options.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <db_connector/query/query.h>
#include <db_connector/settings.h>

#include <query/query_manager.h>

#include "queries_storage_type.h"

namespace srv
{
namespace db
{
namespace qry
{

class QueryManager : public srv::tracer::TracerProvider, public IQueryManager
{
public:
    QueryManager(const DBConnectorSettings& settings, const std::shared_ptr<srv::IServiceLocator>& locator);

    std::unique_ptr<IQueriesLock> GetQueries() override;

    uint64_t GetOrSupportQueryId(std::unique_ptr<IQuery>&& query) override;

private:
    std::mutex m_queriesMutex;
    uint64_t m_currentId = 0;
    queriesStorage_t m_supportedQueries;
};

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_553200B0_A64F_46F4_9F8D_F9853B75257B