#ifndef H_9C71A225_6ACD_431F_9474_3BAFE1DBB213
#define H_9C71A225_6ACD_431F_9474_3BAFE1DBB213

#include <memory>
#include <shared_mutex>

#include <db_connector/accessor.h>
#include <tracer/tracer_provider.h>

namespace srv
{
namespace db
{

class ConnectionOptions : public srv::tracer::TracerProvider
{
public:
    ConnectionOptions(std::shared_ptr<srv::ITracer> tracer);

    void SetSettings(const DBConnectorSettings& settings);

    const std::string& GetConnectionString() const;

private:
    // for constructing connections string
    static constexpr std::string_view ADDRESS_CONN = "host";
    static constexpr std::string_view PORT_CONN = "port";
    static constexpr std::string_view DBNAME_CONN = "dbname";
    static constexpr std::string_view USER_CONN = "user";
    static constexpr std::string_view PASSWORD_CONN = "password";

private:
    mutable std::shared_mutex m_optionsMutex;
    mutable std::string m_cachedString;

    std::string m_address;
    uint32_t m_port;
    std::string m_dbname;
    std::string m_user;
    std::string m_password;
};

}  // namespace db
}  // namespace srv

#endif  // H_9C71A225_6ACD_431F_9474_3BAFE1DBB213
