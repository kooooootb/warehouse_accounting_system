#ifndef H_DABCA060_B73E_4ECA_A036_9025F233F4FF
#define H_DABCA060_B73E_4ECA_A036_9025F233F4FF

#include <instrumental/common.h>
#include <instrumental/interface.h>

#include <locator/service_locator.h>

#include <db_connector/settings.h>

#include <db_connector/query/query.h>

namespace srv
{
namespace db
{
namespace qry
{

/**
 * @brief holds array of 'supported' queries
 * @details supported queries are those that should be prepared on all connections
 */
struct IQueryManager : public ufa::IBase
{
    /**
     * @brief query identificator
     */
    using queryid_t = int64_t;

    /**
     * @brief expose query useful properties
     */
    class QueriesIterator
    {
    public:
    private:
    };

    /**
     * @brief allows iterating through queries props
     * @warning implementation locks queries internal mutex so dispose is asayc
     */
    class IQueriesLock : ufa::IBase
    {
        QueriesIterator;
    };

    /**
     * @brief used primarily by connections to refresh their prepared queries
     */
    virtual QueriesLock GetQueries() = 0;

    /**
     * @brief will try to retrive queryid if it was supported, if not - add it to supported array
     */
    virtual uint64_t GetOrSupportQueryId(std::unique_ptr<IQuery>&& query) = 0;

    static std::unique_ptr<IQueryManager> Create(const DBConnectorSettings& settings, const std::shared_ptr<IServiceLocator>& locator);
};

inline IQueryManager::QueriesLock::QueriesLock(std::mutex& queriesMutex,
    const std::map<uint64_t, std::unique_ptr<IQueryOptions>>& queries)
    : m_queries(queries)
    , m_lock(queriesMutex)
{
}

inline const std::map<uint64_t, std::unique_ptr<IQueryOptions>>& IQueryManager::QueriesLock::operator*()
{
    return m_queries;
}

inline const std::map<uint64_t, std::unique_ptr<IQueryOptions>>* IQueryManager::QueriesLock::operator->()
{
    return &m_queries;
}

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_DABCA060_B73E_4ECA_A036_9025F233F4FF