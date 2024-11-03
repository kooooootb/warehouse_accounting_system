#ifndef H_707F306D_60FC_47A8_9359_EED8EEC33AFF
#define H_707F306D_60FC_47A8_9359_EED8EEC33AFF

#include <instrumental/types.h>

#include <query/query_manager.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>

#include <db_connector/transaction.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry_factory.h>
#include <db_connector/transaction_policies.h>

#include <connection/connection.h>
#include <connection/connection_pool.h>

#include "entries/transaction_entry_factory.h"
#include "raw_transaction.h"

namespace srv
{
namespace db
{
namespace trsct
{

class Transaction : public srv::tracer::TracerProvider, public ITransaction
{
public:
    Transaction(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<qry::IQueryManager> queryManager,
        conn::ConnectionProxy&& connection,
        uint32_t commitAttempts,
        Isolation isolation,
        WritePolicy writePolicy);

    void SetRootEntry(std::unique_ptr<ITransactionEntry>&& entry) override;
    ufa::Result Execute() override;
    ITransactionEntryFactory& GetEntriesFactory() override;

private:
    std::shared_ptr<qry::IQueryManager> m_queryManager;

    conn::ConnectionProxy m_connection;

    std::unique_ptr<IRawTransaction> m_transactionImpl;  // uses connection

    std::unique_ptr<ITransactionEntry> m_entry;
    std::unique_ptr<ITransactionEntryFactory> m_entryFactory;

    uint32_t m_commitAttempts;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_707F306D_60FC_47A8_9359_EED8EEC33AFF