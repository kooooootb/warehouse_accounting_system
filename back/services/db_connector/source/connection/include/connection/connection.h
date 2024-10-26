#ifndef H_B0BCBA40_95A9_48E7_8B07_50FA886A3D9B
#define H_B0BCBA40_95A9_48E7_8B07_50FA886A3D9B

#include <pqxx/pqxx>

#include <instrumental/common.h>

#include <query/query_manager.h>

namespace srv
{
namespace db
{
namespace conn
{

/**
 * @warning not thread safe so is underlying pqxx::connection
 */
struct IConnection : ufa::IBase
{
    virtual void RefreshSupportedQueries(qry::IQueryManager& queryManager) = 0;
    virtual uint64_t GetLastSupportedQueryId() = 0;

    virtual pqxx::connection& GetRaw() = 0;
};

}  // namespace conn
}  // namespace db
}  // namespace srv

#endif  // H_B0BCBA40_95A9_48E7_8B07_50FA886A3D9B