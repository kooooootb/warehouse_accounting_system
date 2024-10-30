#include <mutex>
#include <thread>

#include <db_connector/accessor.h>
#include <instrumental/settings.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <connection/connection_pool.h>

#include "connection.h"
#include "connection_pool.h"

namespace srv
{
namespace db
{
namespace conn
{

constexpr uint32_t DEFAULT_CONNECTIONS_COUNT = 4;
constexpr uint32_t DEFAULT_CONNECTION_ATTEMPTS = 50;

ConnectionPool::ConnectionPool(const DBConnectorSettings& settings, const std::shared_ptr<IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_connectionOptions(GetTracer())
    , m_connectionAttempts(DEFAULT_CONNECTION_ATTEMPTS)
{
    TRACE_INF << TRACE_HEADER;

    SetConnectionsCount(DEFAULT_CONNECTIONS_COUNT);

    SetSettings(settings);
}

std::shared_ptr<IConnectionPool> IConnectionPool::Create(const DBConnectorSettings& settings,
    const std::shared_ptr<srv::IServiceLocator>& locator)
{
    return std::make_shared<ConnectionPool>(settings, locator);
}

void ConnectionPool::SetSettings(const DBConnectorSettings& settings)
{
    TRACE_INF << TRACE_HEADER;

    m_connectionOptions.SetSettings(settings);

    if (settings.connectionCount.has_value())
    {
        SetConnectionsCount(settings.connectionCount.value());
    }

    ufa::TryExtractFromOptional(settings.connectAttempts, m_connectionAttempts);
}

IConnectionPool::ConnectionProxy ConnectionPool::GetConnection()
{
    TRACE_INF << TRACE_HEADER;

    std::lock_guard lock(m_connectionMutex);

    if (!m_freeConnections.empty())
    {
        // we have free connection so just take it
        TRACE_INF << TRACE_HEADER << "Returning connection from free";

        auto connection = std::move(m_freeConnections.back());
        m_freeConnections.pop_back();

        return {shared_from_this(), std::move(connection)};
    }
    else if (m_toCreate > 0)
    {
        // initialize connection from number of uinitialized quota
        TRACE_INF << TRACE_HEADER << "Returning new under quota";

        auto connection = CreateConnection();
        m_toCreate -= 1;

        return {shared_from_this(), std::move(connection)};
    }
    else
    {
        // create new connection and expect it to be deleted after use
        TRACE_INF << TRACE_HEADER << "Returning new and expect it to be deleted";

        auto connection = CreateConnection();
        m_toDelete += 1;

        return {shared_from_this(), std::move(connection)};
    }
}

void ConnectionPool::ReturnConnection(std::unique_ptr<IConnection>&& connection)
{
    TRACE_INF << TRACE_HEADER;

    std::lock_guard lock(m_connectionMutex);

    if (m_toDelete > 0)
    {
        // abandon connection
        TRACE_INF << TRACE_HEADER << "Abandon connection";

        m_toDelete -= 1;
    }
    else
    {
        // return it to free connections
        TRACE_INF << TRACE_HEADER << "Return to free";

        m_freeConnections.push_back(std::move(connection));
    }
}

void ConnectionPool::SetConnectionsCount(uint32_t connectionsCount)
{
    TRACE_INF << TRACE_HEADER << connectionsCount;

    std::lock_guard lock(m_connectionMutex);

    int32_t diff = connectionsCount - (m_freeConnections.size() + m_busy + m_toCreate);

    if (diff > 0)
    {
        m_toCreate += diff;
    }
    else if (diff < 0)
    {
        // convert diff to how much to cut
        diff = -diff;

        // cut uinitialized connections
        int32_t savedDiff = diff;
        diff -= m_toCreate;
        m_toCreate = diff >= 0 ? 0 : m_toCreate - savedDiff;

        if (diff > 0)
        {
            // cut free connections
            savedDiff = diff;
            diff -= m_freeConnections.size();
            m_freeConnections.resize(diff >= 0 ? 0 : m_freeConnections.size() - savedDiff);

            if (diff > 0)
            {
                m_toDelete += diff;
            }
        }
    }
}

std::unique_ptr<IConnection> ConnectionPool::CreateConnection()
{
    TRACE_INF << TRACE_HEADER;

    std::chrono::duration Timeout = std::chrono::seconds(2);

    uint32_t attempt = 0;

    do
    {
        try
        {
            return std::make_unique<Connection>(GetTracer(), m_connectionOptions);
        }
        catch (const std::exception& ex)
        {
            TRACE_WRN << TRACE_HEADER << "Failed connecting to db, error: " << ex.what() << ", attempt : " << attempt;
            std::this_thread::sleep_for(Timeout);
        }
    } while (++attempt <= m_connectionAttempts);

    CHECK_SUCCESS(ufa::Result::NO_CONNECTION,
        "Connection to db failed after " << attempt << " retries, connection string: " << m_connectionOptions.GetConnectionString());
}

}  // namespace conn
}  // namespace db
}  // namespace srv
