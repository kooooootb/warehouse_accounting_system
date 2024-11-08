#include <instrumental/string_converters.h>
#include <tracer/tracer_provider.h>

#include "raw_query_transaction_entry.h"

namespace srv
{
namespace db
{
namespace trsct
{

RawQueryTransactionEntry::RawQueryTransactionEntry(std::shared_ptr<srv::ITracer> tracer,
    IRawTransaction* transaction,
    std::string queryParametrized,
    params_t params,
    result_t* result)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_transaction(transaction)
    , m_queryParametrized(std::move(queryParametrized))
    , m_params(std::move(params))
    , m_result(result)
{
    TRACE_INF << TRACE_HEADER;
}

void RawQueryTransactionEntry::Execute()
{
    TRACE_INF << TRACE_HEADER << "Executing query, parametrized: " << m_queryParametrized << ", params: ["
              << string_converters::ToString(m_params) << "]";

    *m_result = m_transaction->Get()->exec_params(m_queryParametrized, m_params.ToPqxx());
}

ITransactionEntry* RawQueryTransactionEntry::GetNext()
{
    TRACE_INF << TRACE_HEADER << "Next entry ptr: " << m_nextEntry.get();

    return m_nextEntry.get();
}

void RawQueryTransactionEntry::SetNext(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER << "Entry ptr: " << entry.get();

    m_nextEntry = std::move(entry);
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
