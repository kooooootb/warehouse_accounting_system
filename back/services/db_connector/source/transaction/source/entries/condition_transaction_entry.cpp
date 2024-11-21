#include <instrumental/string_converters.h>
#include <tracer/tracer_provider.h>

#include "condition_transaction_entry.h"

namespace srv
{
namespace db
{
namespace trsct
{

ConditionTransactionEntry::ConditionTransactionEntry(std::shared_ptr<srv::ITracer> tracer, std::function<bool()>&& predicate)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_predicate(std::move(predicate))
{
    TRACE_DBG << TRACE_HEADER;
}

void ConditionTransactionEntry::Execute()
{
    TRACE_DBG << TRACE_HEADER;

    m_predicateResult = m_predicate();

    TRACE_INF << TRACE_HEADER << "result: " << m_predicateResult;
}

ITransactionEntry* ConditionTransactionEntry::GetNext()
{
    TRACE_INF << TRACE_HEADER << "Choosing " << (m_predicateResult ? "true" : "false") << "path";

    return m_predicateResult ? m_ifTrueEntry.get() : m_ifFalseEntry.get();
}

void ConditionTransactionEntry::SetIfTrue(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER << "Entry ptr: " << entry.get();

    m_ifTrueEntry = std::move(entry);
}

void ConditionTransactionEntry::SetIfFalse(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER << "Entry ptr: " << entry.get();

    m_ifFalseEntry = std::move(entry);
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
