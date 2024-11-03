#include <instrumental/check.h>
#include <instrumental/settings.h>
#include <instrumental/types.h>
#include <tracer/tracer_provider.h>

#include <db_connector/transaction_policies.h>

#include <transaction/transaction_factory.h>

#include "transaction.h"
#include "transaction_factory.h"

namespace srv
{
namespace db
{
namespace trsct
{

TransactionFactory::TransactionFactory(const DBConnectorSettings& settings,
    const std::shared_ptr<srv::IServiceLocator>& locator,
    std::shared_ptr<conn::IConnectionPool> connectionPool,
    std::shared_ptr<qry::IQueryManager> queryManager)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_connectionPool(connectionPool)
    , m_queryManager(queryManager)
{
    TRACE_INF << TRACE_HEADER;

    SetSettings(settings);
}

std::unique_ptr<ITransactionFactory> ITransactionFactory::Create(const DBConnectorSettings& settings,
    const std::shared_ptr<srv::IServiceLocator>& locator,
    std::shared_ptr<conn::IConnectionPool> connectionPool,
    std::shared_ptr<qry::IQueryManager> queryManager)
{
    return std::make_unique<TransactionFactory>(settings, locator, std::move(connectionPool), std::move(queryManager));
}

std::unique_ptr<ITransaction> TransactionFactory::CreateTransaction(WritePolicy writePolicy, Isolation isolationLevel)
{
    TRACE_INF << TRACE_HEADER << "write policy: " << writePolicy << ", isolation level: " << isolationLevel;

    auto connectionProxy = m_connectionPool->GetConnection();

    return std::make_unique<Transaction>(GetTracer(),
        m_queryManager,
        std::move(connectionProxy),
        m_commitAttempts,
        isolationLevel,
        writePolicy);
}

void TransactionFactory::SetSettings(const DBConnectorSettings& settings)
{
    TRACE_INF << TRACE_HEADER;

    ufa::TryExtractFromOptional(settings.commitAttempts, m_commitAttempts);
}

}  // namespace trsct
}  // namespace db
}  // namespace srv
