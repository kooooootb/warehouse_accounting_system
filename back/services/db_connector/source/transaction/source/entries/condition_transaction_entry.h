#ifndef H_4D2FD922_F265_488B_A671_7331EA9DCBE2
#define H_4D2FD922_F265_488B_A671_7331EA9DCBE2

#include <tracer/tracer_provider.h>

#include <db_connector/transaction_entry/condition_transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry.h>

namespace srv
{
namespace db
{
namespace trsct
{

class ConditionTransactionEntry : public srv::tracer::TracerProvider, public IConditionTransactionEntry
{
public:
    ConditionTransactionEntry(std::shared_ptr<srv::ITracer> tracer, std::function<bool()>&& predicate);

    // ITransactionEntry
    void Execute() override;
    ITransactionEntry* GetNext() override;

    // IConditionTransactionEntry
    void SetIfTrue(std::unique_ptr<ITransactionEntry>&& entry) override;
    void SetIfFalse(std::unique_ptr<ITransactionEntry>&& entry) override;

private:
    std::function<bool()> m_predicate;

    bool m_predicateResult;
    std::unique_ptr<ITransactionEntry> m_ifTrueEntry;
    std::unique_ptr<ITransactionEntry> m_ifFalseEntry;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_4D2FD922_F265_488B_A671_7331EA9DCBE2