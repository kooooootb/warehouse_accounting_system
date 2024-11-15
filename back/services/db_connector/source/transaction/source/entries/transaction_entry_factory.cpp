#include <tracer/tracer_provider.h>

#include "cached_query_transaction_entry.h"
#include "condition_transaction_entry.h"
#include "raw_query_transaction_entry.h"
#include "transaction_entry_factory.h"
#include "variable_transaction_entry.h"

namespace srv
{
namespace db
{
namespace trsct
{

TransactionEntryFactory::TransactionEntryFactory(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<qry::IQueryManager> queryManager,
    conn::IConnection* connection,
    IRawTransaction* transaction)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_queryManager(std::move(queryManager))
    , m_connection(connection)
    , m_transaction(transaction)
{
    TRACE_INF << TRACE_HEADER;
}

std::unique_ptr<IQueryTransactionEntry> TransactionEntryFactory::CreateQueryTransactionEntry(std::unique_ptr<IQuery>&& query,
    bool isCached,
    result_t* result)
{
    TRACE_INF << TRACE_HEADER << "isCached: " << isCached;

    if (isCached)
    {
        const auto params = query->ExtractParams();
        const auto queryId = m_queryManager->GetOrSupportQueryId(std::move(query));

        if (m_connection->GetLastSupportedQueryId() < queryId)
        {
            m_connection->RefreshSupportedQueries(*m_queryManager);
        }

        return std::make_unique<CachedQueryTransactionEntry>(GetTracer(), m_transaction, queryId, std::move(params), result);
    }
    else
    {
        const auto params = query->ExtractParams();
        const auto queryParametrized = query->ExtractOptions()->SerializeParametrized();

        return std::make_unique<RawQueryTransactionEntry>(GetTracer(), m_transaction, queryParametrized, std::move(params), result);
    }
}

std::unique_ptr<IConditionTransactionEntry> TransactionEntryFactory::CreateConditionTransactionEntry(std::function<bool()>&& predicate)
{
    TRACE_INF << TRACE_HEADER;

    return std::make_unique<ConditionTransactionEntry>(GetTracer(), std::move(predicate));
}

std::unique_ptr<IVariableTransactionEntry> TransactionEntryFactory::CreateVariableTransactionEntry(std::function<void()>&& lastEntry)
{
    TRACE_INF << TRACE_HEADER;

    return std::make_unique<VariableTransactionEntry>(GetTracer(), std::move(lastEntry));
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
