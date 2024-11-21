#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <tracer/tracer_provider.h>

#include "cached_query_transaction_entry.h"

namespace srv
{
namespace db
{
namespace trsct
{

CachedQueryTransactionEntry::CachedQueryTransactionEntry(std::shared_ptr<srv::ITracer> tracer,
    IRawTransaction* transaction,
    qry::queryid_t queryId,
    params_t params,
    result_t* result)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_transaction(transaction)
    , m_queryId(queryId)
    , m_params(std::move(params))
    , m_result(result)
{
    TRACE_DBG << TRACE_HEADER;
}

void CachedQueryTransactionEntry::Execute()
{
    TRACE_INF << TRACE_HEADER << "Executing query, id: " << m_queryId << ", params: [" << string_converters::ToString(m_params) << "]";

    result_t result;
    result = m_transaction->Get()->exec_prepared(string_converters::ToString(m_queryId), m_params.ToPqxx());
    if (m_result != nullptr)
    {
        *m_result = std::move(result);
    }
}

ITransactionEntry* CachedQueryTransactionEntry::GetNext()
{
    TRACE_INF << TRACE_HEADER << "Next entry ptr: " << m_nextEntry.get();

    return m_nextEntry.get();
}

void CachedQueryTransactionEntry::SetNext(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER << "Entry ptr: " << entry.get();

    m_nextEntry = std::move(entry);
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
