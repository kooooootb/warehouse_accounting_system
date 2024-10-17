#include <chrono>
#include <exception>
#include <shared_mutex>
#include <sstream>
#include <thread>

#include <fmt/core.h>

#include <db_connector/accessor.h>
#include <instrumental/check.h>
#include <instrumental/common.h>
#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <settings_provider/settings_provider.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>

#include "accessor.h"

#include "scripts/check_valid.h"
#include "scripts/initialize.h"

namespace srv
{
namespace db
{

constexpr bool DEFAULT_ALWAYS_REINITIALIZE = true;
constexpr uint32_t DEFAULT_CONNECTION_ATTEMPTS = 50;

Accessor::Accessor(const std::shared_ptr<srv::IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_connectionOptions(GetTracer())
    , m_connectAttempts(DEFAULT_CONNECTION_ATTEMPTS)
    , m_alwaysReinitialize(DEFAULT_ALWAYS_REINITIALIZE)
{
    TRACE_INF << TRACE_HEADER;

    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(locator->GetInterface(settingsProvider));

    AccessorSettings settings;
    settingsProvider->FillSettings(settings);

    SetSettings(std::move(settings));

    if (DBNeedsReinitializing())
    {
        InitializeDB();
    }
}

void Accessor::SetSettings(const AccessorSettings& settings)
{
    m_connectionOptions.SetSettings(settings);

    {
        std::shared_lock lock(m_optionsMutex);

        ufa::TryExtractFromOptional(settings.connectAttempts, m_connectAttempts);
        ufa::TryExtractFromOptional(settings.alwaysReinitialize, m_alwaysReinitialize);
    }
}

bool Accessor::DBNeedsReinitializing()
{
    {
        std::shared_lock lock(m_optionsMutex);
        if (m_alwaysReinitialize)
        {
            return true;
        }
    }

    auto conn = CreateConnection();
    pqxx::work work(conn);

    auto [count] = work.query1<int>(CHECK_VALID.data());
    work.commit();

    if (count != 1)
        return true;
    else
        return false;
}

void Accessor::InitializeDB()
{
    auto conn = CreateConnection();
    pqxx::work work(conn);

    work.exec(INITIALIZE_DB);
    work.commit();
}

pqxx::connection Accessor::CreateConnection()
{
    constexpr std::chrono::duration Timeout = std::chrono::seconds(2);

    uint32_t attempt = 0;

    do
    {
        try
        {
            return pqxx::connection(m_connectionOptions.GetConnectionString());
        }
        catch (const std::exception& ex)
        {
            TRACE_WRN << TRACE_HEADER << "Failed connecting to db, attempt: " << attempt;
            std::this_thread::sleep_for(Timeout);
        }
    } while (++attempt <= m_connectAttempts);

    CHECK_SUCCESS(ufa::Result::NO_CONNECTION,
        "Connection to db failed after " << attempt << " retries, connection string: " << m_connectionOptions.GetConnectionString());
}

std::string Accessor::JoinFilters(const std::vector<std::string>& filters)
{
    std::stringstream result;
    for (auto it = filters.cbegin(), end = filters.cend() - 1; it != end; ++it)
    {
        result << *it << " AND ";
    }
    result << filters.back();

    return result.str();
}

}  // namespace db
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IAccessor, srv::db::Accessor);
