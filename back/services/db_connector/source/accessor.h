#ifndef H_F9596188_7593_4B03_AB36_79A2E56EBD07
#define H_F9596188_7593_4B03_AB36_79A2E56EBD07

#include <shared_mutex>

#include <pqxx/pqxx>

#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace srv
{
namespace db
{

class ConnectionOptions : public srv::tracer::TracerProvider
{
public:
    ConnectionOptions(std::shared_ptr<srv::ITracer> tracer);

    void SetSettings(const AccessorSettings& settings);

    const std::string& GetConnectionString() const;

private:
    // for constructing connections string
    static constexpr std::string_view ADDRESS_CONN = "host";
    static constexpr std::string_view PORT_CONN = "port";
    static constexpr std::string_view DBNAME_CONN = "dbname";
    static constexpr std::string_view USER_CONN = "user";
    static constexpr std::string_view PASSWORD_CONN = "password";

private:
    std::shared_mutex m_optionsMutex;
    mutable std::string m_cachedString;

    std::string m_address;
    uint32_t m_port;
    std::string m_dbname;
    std::string m_user;
    std::string m_password;
};

class Accessor : public srv::tracer::TracerProvider, public srv::IAccessor
{
public:
    Accessor(const std::shared_ptr<srv::IServiceLocator>& locator);

private:
    void SetSettings(const AccessorSettings& settings);

    bool DBNeedsReinitializing();
    void InitializeDB();
    pqxx::connection CreateConnection();

    std::string JoinFilters(const std::vector<std::string>& filters);

private:
    ConnectionOptions m_connectionOptions;

    // options
    std::shared_mutex m_optionsMutex;
    uint32_t m_connectAttempts;
    bool m_alwaysReinitialize;
};

}  // namespace db
}  // namespace srv

#endif  // H_F9596188_7593_4B03_AB36_79A2E56EBD07