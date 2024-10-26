#ifndef H_90624E80_F63E_41B9_AC6F_E5F8388D1754
#define H_90624E80_F63E_41B9_AC6F_E5F8388D1754

#include <query/query_manager.h>
#include <type_traits>

#include "queries_storage_type.h"

namespace srv
{
namespace db
{
namespace qry
{

// hide using under namespace with complex name as we use it in header file
namespace usdtl
{
using iterator_t = queriesStorage_t::mapped_type::const_iterator;
using reverse_iterator_t = queriesStorage_t::mapped_type::const_reverse_iterator;
}  // namespace usdtl

template <typename IteratorT>
class QueriesIteratorInternal : public IQueryManager::QueriesIterator::IQueriesIteratorInternal
{
public:
    QueriesIteratorInternal(bool isEnd, const queriesStorage_t& queries);

    IQueryManager::QueriesIterator::Field GetField() override;
    void Next() override;
    bool Equals(const IQueriesIteratorInternal& it) override;

private:
    // define our own functions from template argument
    static IteratorT Begin(const queriesStorage_t::mapped_type& quieries);
    static IteratorT End(const queriesStorage_t::mapped_type& quieries);
    static bool Less(queryid_t id1, queryid_t id2);

private:
    enum class CurrentIteratorState
    {
        Normal,
        PostEnd,
    };

    // this will require manual push for the chosen iterator from vector
    void Ascend(std::vector<IteratorT>& nextIterators);
    void Descend(std::vector<IteratorT>& nextIterators);

private:
    std::vector<IteratorT> m_nextIterators;
    size_t m_currentIteratorIndex;
    IteratorT m_currentIterator;
    CurrentIteratorState m_currentIteratorState =  // @unused
        CurrentIteratorState::Normal;              // marks that currentIterator == m_nextIterators[m_currentIteratorIndex] == end
};

template <>
inline usdtl::iterator_t QueriesIteratorInternal<usdtl::iterator_t>::Begin(const queriesStorage_t::mapped_type& quieries)
{
    return std::cbegin(quieries);
}

template <>
inline usdtl::reverse_iterator_t QueriesIteratorInternal<usdtl::reverse_iterator_t>::Begin(
    const queriesStorage_t::mapped_type& quieries)
{
    return std::crbegin(quieries);
}

template <>
inline usdtl::iterator_t QueriesIteratorInternal<usdtl::iterator_t>::End(const queriesStorage_t::mapped_type& quieries)
{
    return std::cend(quieries);
}

template <>
inline usdtl::reverse_iterator_t QueriesIteratorInternal<usdtl::reverse_iterator_t>::End(const queriesStorage_t::mapped_type& quieries)
{
    return std::crend(quieries);
}

template <>
inline bool QueriesIteratorInternal<usdtl::iterator_t>::Less(queryid_t id1, queryid_t id2)
{
    return id1 < id2;
}

template <>
inline bool QueriesIteratorInternal<usdtl::reverse_iterator_t>::Less(queryid_t id1, queryid_t id2)
{
    // reverse logic as we go backwards now
    return id1 > id2;
}

template <typename IteratorT>
QueriesIteratorInternal<IteratorT>::QueriesIteratorInternal(bool isEnd, const queriesStorage_t& queries)
{
    if (isEnd)
    {
        // make all iterators pre end
        m_nextIterators.reserve(queries.size());
        for (const auto& it : queries)
        {
            m_nextIterators.emplace_back(std::prev(End(it.second)));
        }

        // get iterator with max value
        const auto maxIt = std::max_element(m_nextIterators.begin(),
            m_nextIterators.end(),
            [](const auto& a, const auto& b) -> bool
            {
                return Less(a->first, b->first);
            });

        // get currentIterator as closest post end iterator
        m_currentIterator = std::next(*maxIt);
        m_currentIteratorIndex = maxIt - m_nextIterators.begin();

        // make all iterators post end
        for (auto& it : m_nextIterators)
        {
            it = std::next(it);
        }

        // we can go beyond end so mark current iterator as post end manually
        m_currentIteratorState = CurrentIteratorState::PostEnd;
    }
    else
    {
        // make all iterators begin
        m_nextIterators.reserve(queries.size());
        for (const auto& it : queries)
        {
            m_nextIterators.emplace_back(Begin(it.second));
        }

        // get iterator with min value
        const auto minIt = std::min_element(m_nextIterators.begin(),
            m_nextIterators.end(),
            [](const auto& a, const auto& b) -> bool
            {
                return Less(a->first, b->first);
            });
        m_currentIterator = *minIt;
        m_currentIteratorIndex = minIt - m_nextIterators.begin();

        // bump iterator from nextIterators == current
        *minIt = std::next(*minIt);
    }
}

template <typename IteratorT>
inline IQueryManager::QueriesIterator::Field QueriesIteratorInternal<IteratorT>::GetField()
{
    return {.id = m_currentIterator->first, .queryOptions = m_currentIterator->second.get()};
}

template <typename IteratorT>
inline void QueriesIteratorInternal<IteratorT>::Next()
{
    const auto minIt = std::min_element(m_nextIterators.begin(),
        m_nextIterators.end(),
        [](const auto& a, const auto& b) -> bool
        {
            return Less(a->first, b->first);
        });

    m_currentIterator = *minIt;
    m_currentIteratorIndex = minIt - m_nextIterators.begin();

    *minIt = std::next(*minIt);
}

template <typename IteratorT>
inline bool QueriesIteratorInternal<IteratorT>::Equals(const IQueriesIteratorInternal& it)
{
    return m_currentIterator == static_cast<const QueriesIteratorInternal&>(it).m_currentIterator;
}

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_90624E80_F63E_41B9_AC6F_E5F8388D1754
