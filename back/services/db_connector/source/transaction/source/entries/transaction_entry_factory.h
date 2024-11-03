#ifndef H_7752CFF9_D75F_46AA_8635_9F7E770A9F5C
#define H_7752CFF9_D75F_46AA_8635_9F7E770A9F5C

#include <tracer/tracer_provider.h>

#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/transaction_entry_factory.h>

#include <connection/connection.h>
#include <query/query_manager.h>
#include "back/services/db_connector/source/transaction/source/raw_transaction.h"

namespace srv
{
namespace db
{
namespace trsct
{

class TransactionEntryFactory : public srv::tracer::TracerProvider, public ITransactionEntryFactory
{
public:
    TransactionEntryFactory(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<qry::IQueryManager> queryManager,
        conn::IConnection* connection,
        IRawTransaction* transaction);

    std::unique_ptr<IQueryTransactionEntry> CreateQueryTransactionEntry(std::unique_ptr<IQuery>&& query,
        bool isCached,
        result_t* result) override;
    std::unique_ptr<IConditionTransactionEntry> CreateConditionTransactionEntry(std::function<bool()>&& predicate) override;

private:
    std::shared_ptr<qry::IQueryManager> m_queryManager;
    conn::IConnection* m_connection;  // connection is managed by the caller
    IRawTransaction* m_transaction;   // so is transaction
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_7752CFF9_D75F_46AA_8635_9F7E770A9F5C
