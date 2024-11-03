#ifndef H_D54C1C1A_DA2E_4DFA_B4B6_0E548477A486
#define H_D54C1C1A_DA2E_4DFA_B4B6_0E548477A486

#include <instrumental/common.h>
#include <instrumental/interface.h>

#include <db_connector/transaction.h>
#include <db_connector/transaction_policies.h>

#include <connection/connection_pool.h>
#include <query/query_manager.h>

namespace srv
{
namespace db
{
namespace trsct
{

struct ITransactionFactory : public ufa::IBase
{
    virtual std::unique_ptr<ITransaction> CreateTransaction(WritePolicy writePolicy = WritePolicy::ReadWrite,
        Isolation isolationLevel = Isolation::RepeatableRead) = 0;

    virtual void SetSettings(const DBConnectorSettings& settings) = 0;

    static std::unique_ptr<ITransactionFactory> Create(const DBConnectorSettings& settings,
        const std::shared_ptr<srv::IServiceLocator>& locator,
        std::shared_ptr<conn::IConnectionPool> connectionPool,
        std::shared_ptr<qry::IQueryManager> queryManager);
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_D54C1C1A_DA2E_4DFA_B4B6_0E548477A486
