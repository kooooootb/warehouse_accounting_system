#ifndef H_7F4128E9_9A5A_47EB_B7CC_7C5638D38411
#define H_7F4128E9_9A5A_47EB_B7CC_7C5638D38411

#include <tracer/tracer_provider.h>

#include <transaction/transaction_factory.h>

namespace srv
{
namespace db
{
namespace trsct
{

class TransactionFactory : public srv::tracer::TracerProvider, public ITransactionFactory
{
public:
    TransactionFactory(const DBConnectorSettings& settings,
        const std::shared_ptr<srv::IServiceLocator>& locator,
        std::shared_ptr<conn::IConnectionPool> connectionPool,
        std::shared_ptr<qry::IQueryManager> queryManager);

    std::unique_ptr<ITransaction> CreateTransaction() override;

    void SetSettings(const DBConnectorSettings& settings) override;

private:
    std::shared_ptr<conn::IConnectionPool> m_connectionPool;
    std::shared_ptr<qry::IQueryManager> m_queryManager;
};

}  // namespace trsct
}  // namespace db
}  // namespace srv

#endif  // H_7F4128E9_9A5A_47EB_B7CC_7C5638D38411