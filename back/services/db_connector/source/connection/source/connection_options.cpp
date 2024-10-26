#include "connection_options.h"

namespace srv
{
namespace db
{

constexpr std::string_view DEFAULT_ADDRESS = "127.0.0.1";
constexpr uint32_t DEFAULT_PORT = 5432;
constexpr std::string_view DEFAULT_DBNAME = "postgres";
constexpr std::string_view DEFAULT_USER = "postgres";
constexpr std::string_view DEFAULT_PASSWORD = "defaultpassword";  // doesn't make much sense

ConnectionOptions::ConnectionOptions(std::shared_ptr<srv::ITracer> tracer)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_address(DEFAULT_ADDRESS)
    , m_port(DEFAULT_PORT)
    , m_dbname(DEFAULT_DBNAME)
    , m_user(DEFAULT_USER)
    , m_password(DEFAULT_PASSWORD)
{
}

void ConnectionOptions::SetSettings(const DBConnectorSettings& settings)
{
    std::unique_lock lock(m_optionsMutex);

    ufa::TryExtractFromOptional(settings.dbmsAddress, m_address);
    ufa::TryExtractFromOptional(settings.dbmsPort, m_port);
    ufa::TryExtractFromOptional(settings.dbmsDbname, m_dbname);
    ufa::TryExtractFromOptional(settings.dbmsUser, m_user);
    ufa::TryExtractFromOptional(settings.dbmsPassword, m_password);

    m_cachedString.clear();
}

const std::string& ConnectionOptions::GetConnectionString() const
{
    std::shared_lock readLock(m_optionsMutex);

    if (m_cachedString.empty())
    {
        readLock.unlock();
        std::unique_lock writeLock(m_optionsMutex);

        std::stringstream connString;
        connString << ADDRESS_CONN << '=' << m_address << ' ';
        connString << DBNAME_CONN << '=' << m_dbname << ' ';
        connString << PORT_CONN << '=' << m_port << ' ';
        connString << USER_CONN << '=' << m_user << ' ';
        connString << PASSWORD_CONN << '=' << m_password;

        m_cachedString = connString.str();

        // lock dies here so return in this block
        return m_cachedString;
    }

    return m_cachedString;
}

}  // namespace db
}  // namespace srv
