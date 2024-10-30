#ifndef H_707F306D_60FC_47A8_9359_EED8EEC33AFF
#define H_707F306D_60FC_47A8_9359_EED8EEC33AFF

#include <tracer/tracer_provider.h>

#include <db_connector/transaction.h>
#include <db_connector/transaction_entry/transaction_entry.h>

#include <connection/connection.h>
#include <connection/connection_pool.h>

namespace srv
{
namespace db
{
namespace trsct
{

class Transaction : public srv::tracer::TracerProvider, public ITransaction
{
public:
    Transaction(std::shared_ptr<srv::ITracer> tracer, conn::ConnectionProxy&& connection);

    void SetRootEntry(std::unique_ptr<ITransactionEntry>&& entry) override;
    void Execute() override;

private:
    conn::ConnectionProxy m_connection;
    std::unique_ptr<ITransactionEntry> m_entry;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_707F306D_60FC_47A8_9359_EED8EEC33AFF