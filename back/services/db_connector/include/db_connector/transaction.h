#ifndef H_A9CB86EE_6471_419F_A70C_9E58AA3B0F30
#define H_A9CB86EE_6471_419F_A70C_9E58AA3B0F30

#include <instrumental/common.h>
#include <instrumental/interface.h>

#include "condition_transaction_entry.h"
#include "query_options.h"
#include "query_transaction_entry.h"
#include "transaction_entry.h"

namespace srv
{
namespace db
{

struct ITransaction : public ufa::IBase
{
    virtual void SetRootEntry(std::unique_ptr<ITransactionEntry> entry) = 0;
    virtual void Execute() = 0;

    virtual std::unique_ptr<IQueryTransactionEntry> CreateQueryTransactionEntry(QueryOptions queryOptions) = 0;
    virtual std::unique_ptr<IConditionTransactionEntry> CreateConditionTransactionEntry(std::function<bool()> predicate) = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_A9CB86EE_6471_419F_A70C_9E58AA3B0F30