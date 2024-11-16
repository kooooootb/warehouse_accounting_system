#ifndef H_C774DCB5_C529_4641_9FA6_402BE1E4CE69
#define H_C774DCB5_C529_4641_9FA6_402BE1E4CE69

#include "transaction_entry.h"

namespace srv
{
namespace db
{

/**
 * @brief actually should be named 'simple entry'
 */
struct IQueryTransactionEntry : public ITransactionEntry
{
    virtual void SetNext(std::unique_ptr<ITransactionEntry>&& entry) = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_C774DCB5_C529_4641_9FA6_402BE1E4CE69