#include "queries_lock.h"
#include "queries_storage_type.h"

namespace srv
{
namespace db
{
namespace qry
{

inline QueriesLock::QueriesLock(std::mutex& queriesMutex, const queriesStorage_t& queries) : m_queries(queries), m_lock(queriesMutex)
{
}

IQueryManager::QueriesIterator QueriesLock::begin()
{
    ;
}

IQueryManager::QueriesIterator QueriesLock::end()
{
    ;
}

IQueryManager::QueriesIterator QueriesLock::rbegin()
{
    ;
}

IQueryManager::QueriesIterator QueriesLock::rend()
{
    ;
}

}  // namespace qry
}  // namespace db
}  // namespace srv
