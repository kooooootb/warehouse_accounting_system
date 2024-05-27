#ifndef H_F9596188_7593_4B03_AB36_79A2E56EBD07
#define H_F9596188_7593_4B03_AB36_79A2E56EBD07

#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace db
{

class Accessor : public srv::tracer::TracerProvider, public IAccessor
{
public:
    Accessor(std::shared_ptr<srv::IServiceLocator> locator);

    ufa::Result FillUser(data::User& user) override;
    ufa::Result CreateRequirement(data::Requirement& req) override;

private:
    void FillDefaultSettings(AccessorSettings& settings);
    void AcceptSettings(AccessorSettings&& settings);

    bool DBNeedsReinitializing();
    void InitializeDB();
    std::string JoinFilters(const std::vector<std::string>& filters);

private:
    std::string m_connOptions;
};

}  // namespace db

#endif  // H_F9596188_7593_4B03_AB36_79A2E56EBD07