#ifndef H_D63D7953_D0DB_475E_B31D_36A8131B89C9
#define H_D63D7953_D0DB_475E_B31D_36A8131B89C9

#include "transaction_entry.h"

namespace srv
{
namespace db
{

/**
 * @brief will choose next entry based on condition
 */
struct IConditionTransactionEntry : public ITransactionEntry
{
    virtual void SetIfTrue(std::unique_ptr<ITransactionEntry>&& entry) = 0;
    virtual void SetIfFalse(std::unique_ptr<ITransactionEntry>&& entry) = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_D63D7953_D0DB_475E_B31D_36A8131B89C9