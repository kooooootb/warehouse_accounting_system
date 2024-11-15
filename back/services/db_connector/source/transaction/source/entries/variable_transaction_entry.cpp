#include <instrumental/string_converters.h>
#include <tracer/tracer_provider.h>

#include "variable_transaction_entry.h"

namespace srv
{
namespace db
{
namespace trsct
{

VariableTransactionEntry::VariableTransactionEntry(std::shared_ptr<srv::ITracer> tracer, std::function<void()>&& function)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_function(std::move(function))
{
    TRACE_INF << TRACE_HEADER;
}

void VariableTransactionEntry::Execute()
{
    TRACE_INF << TRACE_HEADER;

    m_function();
}

ITransactionEntry* VariableTransactionEntry::GetNext()
{
    TRACE_INF << TRACE_HEADER;

    return m_nextEntry.get();
}

void VariableTransactionEntry::SetNext(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER << "Entry ptr: " << entry.get();

    m_nextEntry = std::move(entry);
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
