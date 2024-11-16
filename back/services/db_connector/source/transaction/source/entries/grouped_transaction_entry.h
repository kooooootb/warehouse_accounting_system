#ifndef H_E712926C_249D_445E_A413_1CE3B6115A77
#define H_E712926C_249D_445E_A413_1CE3B6115A77

#include <list>

#include <tracer/tracer_provider.h>

#include <db_connector/transaction_entry/query_transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry.h>

namespace srv
{
namespace db
{
namespace trsct
{

class GroupedTransactionEntry : public srv::tracer::TracerProvider, public IQueryTransactionEntry
{
public:
    GroupedTransactionEntry(std::shared_ptr<srv::ITracer> tracer, std::list<std::unique_ptr<ITransactionEntry>>&& entries);

    // ITransactionEntry
    void Execute() override;
    ITransactionEntry* GetNext() override;

    // IQueryTransactionEntry
    void SetNext(std::unique_ptr<ITransactionEntry>&& entry) override;

private:
    std::list<std::unique_ptr<ITransactionEntry>> m_entries;

    std::unique_ptr<ITransactionEntry> m_nextEntry;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_E712926C_249D_445E_A413_1CE3B6115A77