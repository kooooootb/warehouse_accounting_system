#ifndef H_F9596188_7593_4B03_AB36_79A2E56EBD07
#define H_F9596188_7593_4B03_AB36_79A2E56EBD07

#include <pqxx/pqxx>

#include <db_connector/accessor.h>
#include <db_connector/data/requirement.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace db
{

class Accessor : public srv::tracer::TracerProvider, public IAccessor
{
public:
    Accessor(std::shared_ptr<srv::IServiceLocator> locator);

    ufa::Result FillUser(data::User& user) override;

    ufa::Result CreateRequirement(const db::data::User& initiativeUser, data::Requirement& req) override;
    ufa::Result CreateSpecification(const db::data::User& initiativeUser,
        const data::Project& proj,
        data::Specification& spec) override;
    ufa::Result CreateDependency(const db::data::User& initiativeUser, const data::Dependency& dep) override;
    ufa::Result CreateProject(const db::data::User& initiativeUser, data::Project& proj) override;

    ufa::Result AddReqInSpec(const db::data::User& initiativeUser,
        const data::Requirement& req,
        const data::Specification& spec,
        data::Section& sect) override;
    ufa::Result AddUser(const db::data::User& initiativeUser, const data::User& user) override;

    ufa::Result GetProjectInfo(const db::data::User& initiativeUser,
        data::Project& proj,
        std::vector<db::data::Specification>& specs,
        std::vector<db::data::Requirement>& reqs) override;
    ufa::Result GetProjects(const db::data::User& initiativeUser, std::vector<db::data::Project>& projs) override;
    ufa::Result GetDependencies(const db::data::User& initiativeUser,
        const db::data::Requirement& req,
        std::vector<db::data::Dependency>& deps) override;
    ufa::Result GetRequirementsBySpecification(const db::data::User& initiativeUser,
        db::data::Specification& spec,
        std::vector<db::data::Requirement>& reqs) override;

    ufa::Result EditRequirement(const db::data::User& initiativeUser,
        const db::data::Requirement& editReq,
        std::vector<db::data::Requirement>& impacted) override;

    ufa::Result SendToApprove(const data::User& initiativeUser, const db::data::Specification& spec) override;
    ufa::Result ApproveSpecification(const data::User& initiativeUser, const db::data::Approve& approve) override;
    ufa::Result RejectSpecification(const data::User& initiativeUser, const db::data::Reject& reject) override;

private:
    void FillDefaultSettings(AccessorSettings& settings);
    void AcceptSettings(AccessorSettings&& settings);

    bool DBNeedsReinitializing();
    void InitializeDB();
    pqxx::connection CreateConnection();

    std::string JoinFilters(const std::vector<std::string>& filters);
    db::data::User::Role GetRoleById(uint64_t id, pqxx::work& work);

private:
    std::string m_connOptions;
    uint32_t m_connectAttempts = 50;
};

}  // namespace db

#endif  // H_F9596188_7593_4B03_AB36_79A2E56EBD07