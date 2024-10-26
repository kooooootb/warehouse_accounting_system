#include <algorithm>
#include <limits>

#include <query/query_manager.h>

#include "back/services/db_connector/source/query/source/queries_lock.h"
#include "queries_iterator_internal.h"

namespace srv
{
namespace db
{
namespace qry
{

// QueriesIteratorInternal::QueriesIteratorInternal(bool isAscending,
//     size_t currentIteratorIndex,
//     std::vector<iterator_t>&& nextIterators)
//     : m_isAscending(isAscending)
//     , m_nextIterators(std::move(nextIterators))
//     , m_currentIteratorIndex(currentIteratorIndex)
//     , m_currentIterator(m_nextIterators[m_currentIteratorIndex]){...}

QueriesIteratorInternal::QueriesIteratorInternal(bool isAscending, bool isEnd, const queriesStorage_t& queries)
    : m_isAscending(isAscending)
{
    if (isEnd)
    {
        if (isAscending)
        {
            InitializeIteratorsAsLast(queries);

            // get std::next of iterator with max id
            const auto maxIt = std::max_element(m_nextIterators.begin(),
                m_nextIterators.end(),
                [](const auto& a, const auto& b) -> bool
                {
                    return a->first < b->first;
                });
            m_currentIterator = std::next(*maxIt);
            m_currentIteratorIndex = maxIt - m_nextIterators.begin();

            // make all iterators = end
            for (auto& it : m_nextIterators)
            {
                it = std::next(it);
            }

            // we can't go past end so mark that currentIterator == m_nextIterators[m_currentIteratorIndex]
            m_currentIteratorState = CurrentIteratorState::End;
        }
        else  // descending
        {
            InitializeIteratorsAsBegin(queries);

            // get std::prev of iterator with min id
            const auto maxIt = std::min_element(m_nextIterators.begin(),
                m_nextIterators.end(),
                [](const auto& a, const auto& b) -> bool
                {
                    return a->first < b->first;
                });
            m_currentIterator = std::prev(*maxIt);
            m_currentIteratorIndex = maxIt - m_nextIterators.begin();

            // make all iterators = past begin
            for (auto& it : m_nextIterators)
            {
                it = std::prev(it);
            }

            // we can't go past end so mark that currentIterator == m_nextIterators[m_currentIteratorIndex]
            m_currentIteratorState = CurrentIteratorState::End;
        }
    }

    m_nextIterators = std::move(nextIterators);
    m_currentIteratorIndex = currentIteratorIndex;
    m_currentIterator = m_nextIterators[m_currentIteratorIndex];
}

void QueriesIteratorInternal::InitializeIteratorsAsBegin(const queriesStorage_t& queries)
{
    m_nextIterators.reserve(queries.size());
    for (const auto& it : queries)
    {
        m_nextIterators.emplace_back(it.second.begin());
    }
}

void QueriesIteratorInternal::InitializeIteratorsAsLast(const queriesStorage_t& queries)
{
    m_nextIterators.reserve(queries.size());
    for (const auto& it : queries)
    {
        m_nextIterators.emplace_back(std::prev(it.second.end()));
    }
}

IQueryManager::QueriesIterator::Field QueriesIteratorInternal::GetField()
{
    return {.id = m_currentIterator->first, .queryOptions = m_currentIterator->second.get()};
}

void QueriesIteratorInternal::Next()
{
    // use cached iterators for this as Next is more likely than Prev
    if (m_isAscending)
    {
        Ascend(m_nextIterators);
        m_nextIterators[m_currentIteratorIndex] = std::next(m_currentIterator);
    }
    else
    {
        Descend(m_nextIterators);
        m_nextIterators[m_currentIteratorIndex] = std::prev(m_currentIterator);
    }
}

void QueriesIteratorInternal::Prev()
{
    // this is a little bit more complex
    if (m_isAscending)
    {
        // make local copy of iterators
        auto nextIterators = m_nextIterators;

        // reverse iterators array so they point backwards
        for (auto& it : nextIterators)
        {
            it = std::prev(it);
        }

        // bump new_iterator == currentIterator twice
        CHECK_TRUE(nextIterators[m_currentIteratorIndex] == m_currentIterator, "sanity check");
        nextIterators[m_currentIteratorIndex] = std::prev(m_currentIterator);

        // we leave current iterator in any way
        m_nextIterators[m_currentIteratorIndex] = m_currentIterator;

        // work with new copies
        Descend(nextIterators);
    }
    else
    {
        // make local copy of iterators
        auto nextIterators = m_nextIterators;

        // reverse iterators array so they point forward
        for (auto& it : nextIterators)
        {
            it = std::next(it);
        }

        // bump new_iterator == currentIterator twice
        CHECK_TRUE(nextIterators[m_currentIteratorIndex] == m_currentIterator, "sanity check");
        nextIterators[m_currentIteratorIndex] = std::next(m_currentIterator);

        // we leave current iterator in any way
        m_nextIterators[m_currentIteratorIndex] = m_currentIterator;

        // work with new copies
        Ascend(nextIterators);
    }
}

void QueriesIteratorInternal::Ascend(std::vector<iterator_t>& nextIterators)
{
    queryid_t minId = std::numeric_limits<queryid_t>::max();
    auto vecIt = nextIterators.begin();

    for (auto it = nextIterators.begin(); it != nextIterators.end(); ++it)
    {
        if ((*it)->first < minId)
        {
            minId = (*it)->first;
            vecIt = it;
        }
    }

    m_currentIterator = *vecIt;
    m_currentIteratorIndex = vecIt - nextIterators.begin();
}

void QueriesIteratorInternal::Descend(std::vector<iterator_t>& nextIterators)
{
    queryid_t maxId = std::numeric_limits<queryid_t>::min();
    auto vecIt = nextIterators.begin();

    for (auto it = nextIterators.begin(); it != nextIterators.end(); ++it)
    {
        if ((*it)->first > maxId)
        {
            maxId = (*it)->first;
            vecIt = it;
        }
    }

    m_currentIterator = *vecIt;
    m_currentIteratorIndex = vecIt - nextIterators.begin();
}

bool QueriesIteratorInternal::Equals(const IQueriesIteratorInternal& it)
{
    return m_currentIterator == static_cast<const QueriesIteratorInternal&>(it).m_currentIterator;
}

}  // namespace qry
}  // namespace db
}  // namespace srv
