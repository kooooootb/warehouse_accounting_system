#ifndef H_D54C1C1A_DA2E_4DFA_B4B6_0E548477A486
#define H_D54C1C1A_DA2E_4DFA_B4B6_0E548477A486

#include <instrumental/common.h>
#include <instrumental/interface.h>

#include "transaction.h"

namespace srv
{
namespace db
{
namespace trsct
{

struct ITransactionFactory : public ufa::IBase
{
    virtual std::unique_ptr<ITransaction> CreateTransaction() = 0;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_D54C1C1A_DA2E_4DFA_B4B6_0E548477A486
