#ifndef H_6D454501_EACE_433E_9895_B074126F15DB
#define H_6D454501_EACE_433E_9895_B074126F15DB

#include "transaction_entry.h"

namespace srv
{
namespace db
{

/**
 * @brief will execute function
 */
struct IVariableTransactionEntry : public ITransactionEntry
{
    virtual void SetNext(std::unique_ptr<ITransactionEntry>&& entry) = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_6D454501_EACE_433E_9895_B074126F15DB