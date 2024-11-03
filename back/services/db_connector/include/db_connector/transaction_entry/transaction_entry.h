#ifndef H_F716296E_7DD7_4141_B45A_710CDA9F097E
#define H_F716296E_7DD7_4141_B45A_710CDA9F097E

#include <memory>

#include <instrumental/common.h>
#include <instrumental/interface.h>

namespace srv
{
namespace db
{

struct ITransactionEntry : public ufa::IBase
{
    /**
     * @brief execute inner logic like querying db or calculating expressions
     */
    virtual void Execute() = 0;

    /**
     * @brief get next entry which was selected by inner logic
     * @return non owning pointer to entry
     */
    virtual ITransactionEntry* GetNext() = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_F716296E_7DD7_4141_B45A_710CDA9F097E