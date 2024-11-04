#ifndef H_8C7C25E7_91EE_4523_9CD7_DEF65C5CABEE
#define H_8C7C25E7_91EE_4523_9CD7_DEF65C5CABEE

#include <atomic>

#include <tracer/tracer_provider.h>

#include <connection/connection.h>
#include <connection/connection_pool.h>
#include <db_manager/db_manager.h>

namespace srv
{
namespace db
{
namespace dbmgr
{

class DbManager : public srv::tracer::TracerProvider, public IDbManager
{
public:
    DbManager(const DBConnectorSettings& settings,
        const std::shared_ptr<IServiceLocator>& locator,
        std::shared_ptr<conn::IConnectionPool> connectionPool);

    void PrepareDb() override;
    ufa::Result GetState() override;

    void SetSettings(const DBConnectorSettings& settings) override;

private:
    bool IsDbValid(conn::IConnection& connection);         // true if valid
    bool InitializeTables(conn::IConnection& connection);  // true on success
    bool CreateDb();                                       // true on success

private:
    std::shared_ptr<conn::IConnectionPool> m_connectionPool;

    std::mutex m_settingsMutex;
    std::string m_dbname;
    bool m_shouldCreateDb;
    bool m_shouldReinitializeDb;

    std::atomic<bool> m_isDbHealty = false;
};

}  // namespace dbmgr
}  // namespace db
}  // namespace srv

#endif  // H_8C7C25E7_91EE_4523_9CD7_DEF65C5CABEE