#ifndef H_C5FD6F6C_B597_4221_9FFD_5CC3A527C767
#define H_C5FD6F6C_B597_4221_9FFD_5CC3A527C767

#include <tracer/tracer_provider.h>

#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/query_transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry.h>

#include <connection/connection.h>
#include "back/services/db_connector/source/transaction/source/raw_transaction.h"

namespace srv
{
namespace db
{
namespace trsct
{

class RawQueryTransactionEntry : public srv::tracer::TracerProvider, public IQueryTransactionEntry
{
public:
    RawQueryTransactionEntry(std::shared_ptr<srv::ITracer> tracer,
        IRawTransaction* transaction,
        std::string queryParametrized,
        params_t params,
        result_t* result);

    // ITransactionEntry
    void Execute() override;
    ITransactionEntry* GetNext() override;

    // IQueryTransactionEntry
    void SetNext(std::unique_ptr<ITransactionEntry>&& entry) override;

private:
    IRawTransaction* m_transaction;

    std::string m_queryParametrized;
    params_t m_params;
    result_t* m_result;

    std::unique_ptr<ITransactionEntry> m_nextEntry;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_C5FD6F6C_B597_4221_9FFD_5CC3A527C767