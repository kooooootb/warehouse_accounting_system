#include <chrono>
#include <exception>
#include <shared_mutex>
#include <sstream>
#include <thread>

#include <connection/connection_pool.h>
#include <fmt/core.h>

#include <db_connector/accessor.h>
#include <instrumental/check.h>
#include <instrumental/common.h>
#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <query/query_manager.h>
#include <settings_provider/settings_provider.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>
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

    SetSettings(std::move(settings));
}

void Accessor::SetSettings(const db::DBConnectorSettings& settings)
{
    m_connectionPool->SetSettings(settings);
    m_transactionFactory->SetSettings(settings);
}

std::unique_ptr<ITransaction> Accessor::CreateTransaction(WritePolicy writePolicy, Isolation isolation)
{
    return m_transactionFactory->CreateTransaction(writePolicy, isolation);
}

}  // namespace db
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IAccessor, srv::db::Accessor);
