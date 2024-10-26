#ifndef H_90624E80_F63E_41B9_AC6F_E5F8388D1754
#define H_90624E80_F63E_41B9_AC6F_E5F8388D1754

#include <query/query_manager.h>

#include "queries_storage_type.h"

namespace srv
{
namespace db
{
namespace qry
{

class QueriesIteratorInternal : public IQueryManager::QueriesIterator::IQueriesIteratorInternal
{
public:
    using iterator_t = queriesStorage_t::mapped_type::const_iterator;

public:
    QueriesIteratorInternal(bool isAscending, bool isEnd, const queriesStorage_t& queries);

    IQueryManager::QueriesIterator::Field GetField() override;
    void Next() override;
    void Prev() override;
    bool Equals(const IQueriesIteratorInternal& it) override;

private:
    enum class CurrentIteratorState
    {
        End,
        Normal,
        Begin
    };

    void InitializeIteratorsAsBegin(const queriesStorage_t& queries);  // init m_nextIterators as begin of all queries vectors
    void InitializeIteratorsAsLast(const queriesStorage_t& queries);   // init m_nextIterators as std::prev(end of all queries vectors)

    // this will require manual push for the chosen iterator from vector
    void Ascend(std::vector<iterator_t>& nextIterators);
    void Descend(std::vector<iterator_t>& nextIterators);

private:
    const bool m_isAscending;  // true for begin, false for rbegin
    std::vector<iterator_t> m_nextIterators;
    size_t m_currentIteratorIndex;
    iterator_t m_currentIterator;
    CurrentIteratorState m_currentIteratorState = CurrentIteratorState::Normal;
};

}  // namespace qry
}  // namespace db
}  // namespace srv

#endif  // H_90624E80_F63E_41B9_AC6F_E5F8388D1754
