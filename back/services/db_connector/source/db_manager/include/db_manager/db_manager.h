#ifndef H_53D90F09_2B14_4927_85F0_DC95E8778567
#define H_53D90F09_2B14_4927_85F0_DC95E8778567

#include <instrumental/common.h>
#include <instrumental/types.h>

#include <db_connector/settings.h>

#include <connection/connection_pool.h>

namespace srv
{
namespace db
{
namespace dbmgr
{

/**
 * @brief will hold connection pool
 */
struct IDbManager : public ufa::IBase
{
    /**
     * @brief prepare db or set error flag
     * @details there is 3 probable outcomes
     * 1 - specified db is reachable and passes validness check -> use it
     * 2 - specified db is reachable and not passes check -> 
     *          if <shouldReinitializeDb> = true -> reinitialize needed tables and use it
     *          if <shouldReinitializeDb> != true -> set error flag
     * 3 - specified db is not reachable ->
     *          if <shouldCreateDb> = true -> connect to postgres db and try to create db with specified name
     *          if <shouldCreateDb> != true or creating was unsuccessful -> set error flag
     */
    virtual void PrepareDb() = 0;

    /**
     * @brief get current db state
     * @return SUCCESS if db can be used, ERROR if db cannot be used
     */
    virtual ufa::Result GetState() = 0;

    /**
     * @brief set settings, will call preparedb if connection settings were changed
     */
    virtual void SetSettings(const DBConnectorSettings& settings) = 0;

    static std::unique_ptr<IDbManager> Create(const DBConnectorSettings& settings,
        const std::shared_ptr<IServiceLocator>& locator,
        std::shared_ptr<conn::IConnectionPool> connectionPool);
};

}  // namespace dbmgr
}  // namespace db
}  // namespace srv

#endif  // H_53D90F09_2B14_4927_85F0_DC95E8778567