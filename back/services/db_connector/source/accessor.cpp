#include <iostream>

#include <instrumental/check.h>
#include <instrumental/common.h>
#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>

#include <db_connector/accessor.h>
#include <settings_provider/settings_provider.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>

#include <connection/connection_pool.h>
#include <db_manager/db_manager.h>
#include <query/query_manager.h>
#include <transaction/transaction_factory.h>

#include "accessor.h"

#include "scripts/check_valid.h"
#include "scripts/initialize.h"

namespace srv
{
namespace db
{

Accessor::Accessor(const std::shared_ptr<srv::IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
    TRACE_INF << TRACE_HEADER;

    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(locator->GetInterface(settingsProvider));

    DBConnectorSettings settings;
    settingsProvider->FillSettings(settings);

    m_connectionPool = conn::IConnectionPool::Create(settings, locator);
    m_queryManager = qry::IQueryManager::Create(settings, locator);
    m_transactionFactory = trsct::ITransactionFactory::Create(settings, locator, m_connectionPool, m_queryManager);
    m_dbManager = dbmgr::IDbManager::Create(settings, locator, m_connectionPool);

    SetSettings(std::move(settings));
}

void Accessor::SetSettings(const db::DBConnectorSettings& settings)
{
    m_connectionPool->SetSettings(settings);
    m_transactionFactory->SetSettings(settings);
    m_dbManager->SetSettings(settings);
}

ufa::Result Accessor::CreateTransaction(std::unique_ptr<db::ITransaction>& transaction,
    db::WritePolicy writePolicy,
    db::Isolation isolation)
{
    try
    {
        transaction = m_transactionFactory->CreateTransaction(writePolicy, isolation);
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Received exception, waht(): " << ex.what();
        return ufa::Result::NO_CONNECTION;
    }

    return ufa::Result::SUCCESS;
}

ufa::Result Accessor::IsDbValid()
{
    return m_dbManager->GetState();
}

}  // namespace db
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IAccessor, srv::db::Accessor);
