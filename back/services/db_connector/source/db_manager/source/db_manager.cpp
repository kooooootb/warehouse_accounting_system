#include <chrono>
#include <iostream>
#include <thread>

#include <connection/connection_pool.h>
#include <db_connector/settings.h>
#include <db_manager/db_manager.h>
#include <instrumental/settings.h>
#include <instrumental/types.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>
#include <exception>

#include "db_manager.h"

#include "scripts/create_database.h"
#include "scripts/initialize_tables.h"
#include "scripts/is_valid.h"

namespace srv
{
namespace db
{
namespace dbmgr
{

constexpr bool DefaultShouldReinitializeDb = false;
constexpr bool DefaultShouldCreateDb = false;
constexpr std::string_view DefaultDbname = "postgres";
constexpr std::string_view MasterDbname = "postgres";

DbManager::DbManager(const DBConnectorSettings& settings,
    const std::shared_ptr<IServiceLocator>& locator,
    std::shared_ptr<conn::IConnectionPool> connectionPool)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_connectionPool(std::move(connectionPool))
    , m_dbname(DefaultDbname)
    , m_shouldCreateDb(DefaultShouldCreateDb)
    , m_shouldReinitializeDb(DefaultShouldReinitializeDb)
{
    TRACE_INF << TRACE_HEADER;
}

std::unique_ptr<IDbManager> IDbManager::Create(const DBConnectorSettings& settings,
    const std::shared_ptr<IServiceLocator>& locator,
    std::shared_ptr<conn::IConnectionPool> connectionPool)
{
    return std::make_unique<DbManager>(settings, locator, connectionPool);
}

void DbManager::SetSettings(const DBConnectorSettings& settings)
{
    TRACE_INF << TRACE_HEADER;

    {
        std::lock_guard lock(m_settingsMutex);

        ufa::TryExtractFromOptional(settings.shouldCreateDb, m_shouldCreateDb);
        ufa::TryExtractFromOptional(settings.shouldReinitializeDb, m_shouldReinitializeDb);
        ufa::TryExtractFromOptional(settings.dbmsDbname, m_dbname);
    }

    if (settings.dbmsAddress.has_value() || settings.dbmsPort.has_value() || settings.dbmsDbname.has_value() ||
        settings.dbmsUser.has_value() || settings.dbmsPassword.has_value())
    {
        PrepareDb();
    }
}

void DbManager::PrepareDb()
{
    TRACE_INF << TRACE_HEADER;

    try
    {
        auto connectionProxy = m_connectionPool->GetConnection();

        // 1 or 2 outcome
        auto& connection = connectionProxy.GetConnection();
        if (IsDbValid(connection))
        {
            // 1 outcome
            TRACE_INF << TRACE_HEADER << "Db is reachable and valid";
            m_isDbHealty = true;
        }
        else
        {
            // 2 outcome
            TRACE_INF << TRACE_HEADER << "Db is reachable and not valid, reinitialize";
            m_isDbHealty = InitializeTables(connection);
        }
    }
    catch (const std::exception& ex)
    {
        // 3 outcome
        TRACE_ERR << TRACE_HEADER << "Db is not reachable, try to create it";
        m_isDbHealty = CreateDb();
    }
}

ufa::Result DbManager::GetState()
{
    TRACE_INF << TRACE_HEADER << "Is db healthy: " << m_isDbHealty.load();

    return m_isDbHealty ? ufa::Result::SUCCESS : ufa::Result::ERROR;
}

bool DbManager::IsDbValid(conn::IConnection& connection)
{
    TRACE_INF << TRACE_HEADER;

    try
    {
        pqxx::nontransaction transaction(connection.GetRaw());

        const auto resultRow = transaction.exec1(pqxx::zview(scpts::IsValid));

        CHECK_TRUE(resultRow.size() == 1);

        bool result;
        resultRow.front().to(result);
        CHECK_TRUE(result);
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Check validity failed, what(): " << ex.what();
        return false;
    }

    TRACE_INF << TRACE_HEADER << "Db is valid";
    return true;
}

bool DbManager::CreateDb()
{
    TRACE_INF << TRACE_HEADER;
    std::lock_guard lock(m_settingsMutex);

    if (!m_shouldCreateDb)
    {
        return false;
    }

    try
    {
        // connect to master db
        DBConnectorSettings settings;
        settings.dbmsDbname = MasterDbname;
        m_connectionPool->SetSettings(settings);

        {
            // run query
            auto connectionProxy = m_connectionPool->GetConnection();
            pqxx::nontransaction transaction(connectionProxy.GetConnection().GetRaw());
            transaction.exec0(scpts::CreateDatabase(m_dbname));
        }

        // reset connection to specified db
        settings.dbmsDbname = m_dbname;
        m_connectionPool->SetSettings(settings);

        {
            // initialize new db
            auto connectionProxy = m_connectionPool->GetConnection();
            CHECK_TRUE(InitializeTables(connectionProxy.GetConnection()));
        }

        {
            // check if new db is valid
            auto connectionProxy = m_connectionPool->GetConnection();
            CHECK_TRUE(IsDbValid(connectionProxy.GetConnection()));
        }
    }
    catch (const std::exception& ex)
    {
        DBConnectorSettings settings;
        settings.dbmsDbname = m_dbname;
        m_connectionPool->SetSettings(settings);

        TRACE_ERR << TRACE_HEADER << "Creating db failed, what(): " << ex.what();
        return false;
    }

    TRACE_INF << TRACE_HEADER << "Db is created successfully";
    return true;
}

bool DbManager::InitializeTables(conn::IConnection& connection)
{
    TRACE_INF << TRACE_HEADER;

    try
    {
        pqxx::nontransaction transaction(connection.GetRaw());
        transaction.exec(pqxx::zview(scpts::InitializeTables));
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Tables initialization failed, what(): " << ex.what();
        return false;
    }

    TRACE_INF << TRACE_HEADER << "Tables initialized";
    return true;
}

}  // namespace dbmgr
}  // namespace db
}  // namespace srv
