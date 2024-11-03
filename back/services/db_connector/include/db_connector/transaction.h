#ifndef H_A9CB86EE_6471_419F_A70C_9E58AA3B0F30
#define H_A9CB86EE_6471_419F_A70C_9E58AA3B0F30

#include <db_connector/query/query.h>
#include <instrumental/common.h>
#include <instrumental/interface.h>

#include "transaction_entry/transaction_entry.h"
#include "transaction_entry/transaction_entry_factory.h"

namespace srv
{
namespace db
{

struct ITransaction : public ufa::IBase
{
    /**
     * @brief set entry from which transaction will being executing
     */
    virtual void SetRootEntry(std::unique_ptr<ITransactionEntry>&& entry) = 0;

    /**
     * @brief execute all entries
     * @return SUCCESS on success, ERROR on db error, like lost connection
     */
    virtual ufa::Result Execute() = 0;

    /**
     * @brief this factory will create entries wich will be executed in terms on this transaction
     * @warning factory is owned by transaction and is destroyed with transaction
     */
    virtual ITransactionEntryFactory& GetEntriesFactory() = 0;
};

}  // namespace db
}  // namespace srv

#endif  // H_A9CB86EE_6471_419F_A70C_9E58AA3B0F30