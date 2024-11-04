#ifndef H_F9596188_7593_4B03_AB36_79A2E56EBD07
#define H_F9596188_7593_4B03_AB36_79A2E56EBD07

#include <shared_mutex>

#include <pqxx/pqxx>

#include <instrumental/types.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <db_connector/accessor.h>

#include <connection/connection_pool.h>
#include <db_manager/db_manager.h>
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

    ufa::Result CreateTransaction(std::unique_ptr<db::ITransaction>& transaction,
        db::WritePolicy writePolicy,
        db::Isolation isolation) override;

    ufa::Result IsDbValid() override;

private:
    std::shared_ptr<conn::IConnectionPool> m_connectionPool;
    std::shared_ptr<qry::IQueryManager> m_queryManager;
    std::shared_ptr<trsct::ITransactionFactory> m_transactionFactory;
    std::shared_ptr<dbmgr::IDbManager> m_dbManager;
};

}  // namespace db
}  // namespace srv

#endif  // H_F9596188_7593_4B03_AB36_79A2E56EBD07