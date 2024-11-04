#include "queries_lock.h"
#include "queries_iterator_internal.h"
#include "queries_storage_type.h"

namespace srv
{
namespace db
{
namespace qry
{

QueriesLock::QueriesLock(std::mutex& queriesMutex, const queriesStorage_t& queries) : m_queries(queries), m_lock(queriesMutex) {}

IQueryManager::QueriesIterator QueriesLock::begin()
{
    return {std::make_unique<QueriesIteratorInternal<usdtl::iterator_t>>(false, m_queries)};
}

IQueryManager::QueriesIterator QueriesLock::end()
{
    return {std::make_unique<QueriesIteratorInternal<usdtl::iterator_t>>(true, m_queries)};
}

IQueryManager::QueriesIterator QueriesLock::rbegin()
{
    return {std::make_unique<QueriesIteratorInternal<usdtl::reverse_iterator_t>>(false, m_queries)};
}

IQueryManager::QueriesIterator QueriesLock::rend()
{
    return {std::make_unique<QueriesIteratorInternal<usdtl::reverse_iterator_t>>(true, m_queries)};
}

}  // namespace qry
}  // namespace db
}  // namespace srv
