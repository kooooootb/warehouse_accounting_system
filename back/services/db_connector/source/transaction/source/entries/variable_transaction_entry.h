#ifndef H_DF934BB2_7786_4FB9_9924_38F9B74C87C6
#define H_DF934BB2_7786_4FB9_9924_38F9B74C87C6

#include <tracer/tracer_provider.h>

#include <db_connector/transaction_entry/transaction_entry.h>
#include <db_connector/transaction_entry/variable_transaction_entry.h>

namespace srv
{
namespace db
{
namespace trsct
{

class VariableTransactionEntry : public srv::tracer::TracerProvider, public IVariableTransactionEntry
{
public:
    VariableTransactionEntry(std::shared_ptr<srv::ITracer> tracer, std::function<void()>&& lastEntry);

    // ITransactionEntry
    void Execute() override;
    ITransactionEntry* GetNext() override;

    // IVariableTransactionEntry
    void SetNext(std::unique_ptr<ITransactionEntry>&& entry) override;

private:
    std::function<void()> m_function;

    std::unique_ptr<ITransactionEntry> m_nextEntry;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_DF934BB2_7786_4FB9_9924_38F9B74C87C6