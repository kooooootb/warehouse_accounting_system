#ifndef H_46F894D0_C82F_40C1_ADBA_60E0598CA9EA
#define H_46F894D0_C82F_40C1_ADBA_60E0598CA9EA

#include <pqxx/pqxx>

#include <instrumental/common.h>

#include <db_connector/accessor.h>
#include <locator/service_locator.h>

#include "connection.h"

namespace srv
{
namespace db
{
namespace conn
{

class IConnectionPool : public ufa::IBase
{
public:
    class ConnectionProxy
    {
    public:
        ConnectionProxy(IConnectionPool* pool, std::unique_ptr<IConnection>&& connection);

        ~ConnectionProxy() noexcept;

        IConnection& GetConnection();
        IConnection& operator*();
        IConnection& operator->();

    private:
        IConnectionPool* m_pool;
        std::unique_ptr<IConnection> m_connection;
    };

    virtual ConnectionProxy GetConnection() = 0;
    virtual void SetSettings(const DBConnectorSettings& settings) = 0;

    static std::unique_ptr<IConnectionPool> Create(const DBConnectorSettings& settings,
        const std::shared_ptr<srv::IServiceLocator>& locator);

private:
    virtual void ReturnConnection(std::unique_ptr<IConnection>&& connection) = 0;
};

inline IConnectionPool::ConnectionProxy::ConnectionProxy(IConnectionPool* pool, std::unique_ptr<IConnection>&& connection)
    : m_pool(pool)
    , m_connection(std::move(connection))
{
}

inline IConnectionPool::ConnectionProxy::~ConnectionProxy() noexcept
{
    m_pool->ReturnConnection(std::move(m_connection));
}

inline IConnection& IConnectionPool::ConnectionProxy::GetConnection()
{
    return *m_connection;
}

inline IConnection& IConnectionPool::ConnectionProxy::operator*()
{
    return *m_connection;
}

inline IConnection& IConnectionPool::ConnectionProxy::operator->()
{
    return *m_connection;
}

}  // namespace conn
}  // namespace db
}  // namespace srv

#endif  // H_46F894D0_C82F_40C1_ADBA_60E0598CA9EA