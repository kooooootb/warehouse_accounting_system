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

class Accessor : public srv::tracer::TracerProvider, public srv::IAccessor
{
public:
    Accessor(const std::shared_ptr<srv::IServiceLocator>& locator);

private:
    void SetSettings(const DBConnectorSettings& settings);

    bool DBNeedsReinitializing();
    void InitializeDB();
    pqxx::connection CreateConnection();

    std::string JoinFilters(const std::vector<std::string>& filters);

private:
    // options
    std::shared_mutex m_optionsMutex;
    uint32_t m_connectAttempts;
    bool m_alwaysReinitialize;
};

}  // namespace db
}  // namespace srv

#endif  // H_F9596188_7593_4B03_AB36_79A2E56EBD07