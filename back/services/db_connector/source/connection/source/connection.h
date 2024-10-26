#ifndef H_8CA5F9D8_9FA1_40A1_A6C1_F0A051BFFC64
#define H_8CA5F9D8_9FA1_40A1_A6C1_F0A051BFFC64

#include <pqxx/pqxx>
#include <string_view>

#include <instrumental/common.h>

#include <tracer/tracer_provider.h>

#include <connection/connection.h>

#include "connection_options.h"

namespace srv
{
namespace db
{
namespace conn
{

class Connection : public srv::tracer::TracerProvider, public IConnection
{
public:
    // will throw on no connection
    Connection(std::shared_ptr<srv::ITracer> tracer, const ConnectionOptions& options);

    void RefreshSupportedQueries(qry::IQueryManager& queryManager) override;
    uint64_t GetLastSupportedQueryId() override;

    pqxx::connection& GetRaw() override;

private:
    void SupportQuery(uint64_t id, const std::string& parametrizedQuery);

private:
    pqxx::connection m_rawConnection;

    uint64_t m_lastSupportedQuery = 0;
};

}  // namespace conn
}  // namespace db
}  // namespace srv

#endif  // H_8CA5F9D8_9FA1_40A1_A6C1_F0A051BFFC64