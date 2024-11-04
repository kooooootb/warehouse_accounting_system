#ifndef H_CB087953_3294_405A_B767_115465F31B79
#define H_CB087953_3294_405A_B767_115465F31B79

#include <atomic>
#include <memory>

#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

#include <db_connector/accessor.h>

#include <connection/connection_pool.h>

#include "connection_options.h"

namespace srv
{
namespace db
{
namespace conn
{

class ConnectionPool : public srv::tracer::TracerProvider, public IConnectionPool, public std::enable_shared_from_this<ConnectionPool>
{
public:
    ConnectionPool(const DBConnectorSettings& settings, const std::shared_ptr<IServiceLocator>& locator);

    ConnectionProxy GetConnection() override;
    void SetSettings(const DBConnectorSettings& settings) override;

private:
    void ReturnConnection(std::unique_ptr<IConnection>&& connection) override;

private:
    void SetConnectionsCount(uint32_t connectionsCount);

    std::unique_ptr<IConnection> CreateConnection();

private:
    ConnectionOptions m_connectionOptions;
    std::atomic<uint32_t> m_connectionAttempts;

    std::mutex m_connectionMutex;
    std::list<std::unique_ptr<IConnection>> m_freeConnections;  // unused connections under quota

    uint32_t m_toDelete = 0;  // count connections that should be abandoned after use
    uint32_t m_toCreate = 0;  // count uinitialized connections
    uint32_t m_busy = 0;      // count connections that are currently taken
};

}  // namespace conn
}  // namespace db
}  // namespace srv

#endif  // H_CB087953_3294_405A_B767_115465F31B79
