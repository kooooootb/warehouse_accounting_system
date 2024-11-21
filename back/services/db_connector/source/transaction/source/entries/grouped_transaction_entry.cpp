#include <instrumental/string_converters.h>
#include <tracer/tracer_provider.h>

#include "grouped_transaction_entry.h"

namespace srv
{
namespace db
{
namespace trsct
{

GroupedTransactionEntry::GroupedTransactionEntry(std::shared_ptr<srv::ITracer> tracer,
    std::list<std::unique_ptr<ITransactionEntry>>&& entries)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_entries(std::move(entries))
{
    TRACE_DBG << TRACE_HEADER;
}

void GroupedTransactionEntry::Execute()
{
    TRACE_DBG << TRACE_HEADER;

    for (const auto& entry : m_entries)
    {
        entry->Execute();
    }
}

ITransactionEntry* GroupedTransactionEntry::GetNext()
{
    TRACE_DBG << TRACE_HEADER;

    return m_nextEntry.get();
}

void GroupedTransactionEntry::SetNext(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER << "Entry ptr: " << entry.get();

    m_nextEntry = std::move(entry);
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
