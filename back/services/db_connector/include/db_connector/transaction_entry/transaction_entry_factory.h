#ifndef H_D3058CC5_3147_40C1_BCED_4983F67507CF
#define H_D3058CC5_3147_40C1_BCED_4983F67507CF

#include <optional>

#include <instrumental/interface.h>

#include "condition_transaction_entry.h"
#include "query_transaction_entry.h"

namespace srv
{
namespace db
{

/**
 * @brief static class for constructing transactionEntries
 */
struct ITransactionEntryFactory : ufa::IBase
{
    /**
     * @brief this entry will execute query
     * @param isCached if true query will be added to supported and will always be prepared on connections
     * @param result where to store result, empty if no result
     */
    virtual std::unique_ptr<IQueryTransactionEntry> CreateQueryTransactionEntry(std::unique_ptr<IQuery>&& query,
        bool isCached,
        result_t* result) = 0;

    virtual std::unique_ptr<IConditionTransactionEntry> CreateConditionTransactionEntry(std::function<bool()>&& predicate) = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_D3058CC5_3147_40C1_BCED_4983F67507CF
