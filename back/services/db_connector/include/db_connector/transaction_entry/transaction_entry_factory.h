#ifndef H_D3058CC5_3147_40C1_BCED_4983F67507CF
#define H_D3058CC5_3147_40C1_BCED_4983F67507CF

#include "condition_transaction_entry.h"
#include "query_transaction_entry.h"

namespace srv
{
namespace db
{

/**
 * @brief static class for constructing transactionEntries
 */
class TransactionEntryFactory
{
    static std::unique_ptr<IQueryTransactionEntry> CreateQueryTransactionEntry(std::unique_ptr<IQuery>&& query);
    static std::unique_ptr<IConditionTransactionEntry> CreateConditionTransactionEntry(std::function<bool()> predicate);
};

}  // namespace db
}  // namespace srv

#endif  // H_D3058CC5_3147_40C1_BCED_4983F67507CF
