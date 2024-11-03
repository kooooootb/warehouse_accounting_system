#ifndef H_F9596188_7593_4B03_AB36_79A2E56EBD07
#define H_F9596188_7593_4B03_AB36_79A2E56EBD07

#include <shared_mutex>

#include <pqxx/pqxx>

#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <db_connector/accessor.h>

#include <connection/connection_pool.h>
#include <query/query_manager.h>
#include <transaction/transaction_factory.h>

namespace srv
{
namespace db
{

class Accessor : public srv::tracer::TracerProvider, public srv::IAccessor
{
public:
    Accessor(const std::shared_ptr<srv::IServiceLocator>& locator);

    void SetSettings(const db::DBConnectorSettings& settings) override;

    std::unique_ptr<ITransaction> CreateTransaction(WritePolicy writePolicy, Isolation isolation) override;

private:
    std::shared_ptr<conn::IConnectionPool> m_connectionPool;
    std::shared_ptr<qry::IQueryManager> m_queryManager;
    std::shared_ptr<trsct::ITransactionFactory> m_transactionFactory;
};

}  // namespace db
}  // namespace srv

#endif  // H_F9596188_7593_4B03_AB36_79A2E56EBD07