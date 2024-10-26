#ifndef H_D4AFC819_7297_4302_BC08_35144F1768C4
#define H_D4AFC819_7297_4302_BC08_35144F1768C4

#include <db_connector/query/query.h>

#include <query/query_manager.h>

#include "queries_storage_type.h"

namespace srv
{
namespace db
{
namespace qry
{

class QueriesLock : public IQueryManager::IQueriesLock
{
public:
    QueriesLock(std::mutex& queriesMutex, const queriesStorage_t& queries);

    IQueryManager::QueriesIterator begin() override;
    IQueryManager::QueriesIterator end() override;
    IQueryManager::QueriesIterator rbegin() override;
    IQueryManager::QueriesIterator rend() override;

private:
    const queriesStorage_t& m_queries;
    std::lock_guard<std::mutex> m_lock;
};

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_D4AFC819_7297_4302_BC08_35144F1768C4