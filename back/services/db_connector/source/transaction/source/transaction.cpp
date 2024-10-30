#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>

#include "transaction.h"

namespace srv
{
namespace db
{
namespace trsct
{

Transaction::Transaction(std::shared_ptr<srv::ITracer> tracer, conn::ConnectionProxy&& connection)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_connection(std::move(connection))
{
    TRACE_INF << TRACE_HEADER;
}

void Transaction::SetRootEntry(std::unique_ptr<ITransactionEntry>&& entry)
{
    TRACE_INF << TRACE_HEADER;

    m_entry = std::move(entry);
}

void Transaction::Execute()
{
    TRACE_INF << TRACE_HEADER;

    m_entry->Execute();
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
