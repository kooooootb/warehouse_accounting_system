#ifndef H_4E73F803_EC95_45EE_8415_5186BF400488
#define H_4E73F803_EC95_45EE_8415_5186BF400488

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

class CachedQueryTransactionEntry : public srv::tracer::TracerProvider, public IQueryTransactionEntry
{
public:
    CachedQueryTransactionEntry(std::shared_ptr<srv::ITracer> tracer,
        IRawTransaction* transaction,
        qry::queryid_t queryId,
        params_t params,
        result_t* result);

    // ITransactionEntry
    void Execute() override;
    ITransactionEntry* GetNext() override;

    // IQueryTransactionEntry
    void SetNext(std::unique_ptr<ITransactionEntry>&& entry) override;

private:
    IRawTransaction* m_transaction;

    qry::queryid_t m_queryId;
    params_t m_params;
    result_t* m_result;

    std::unique_ptr<ITransactionEntry> m_nextEntry;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_4E73F803_EC95_45EE_8415_5186BF400488