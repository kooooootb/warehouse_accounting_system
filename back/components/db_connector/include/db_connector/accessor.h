#ifndef H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8
#define H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8

#include <db_connector/data/project.h>
#include <instrumental/interface.h>
#include <instrumental/settings.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>

#include <db_connector/data/approve.h>
#include <db_connector/data/dependency.h>
#include <db_connector/data/reject.h>
#include <db_connector/data/requirement.h>
#include <db_connector/data/section.h>
#include <db_connector/data/specification.h>
#include <db_connector/data/user.h>

namespace db
{

constexpr std::string_view DEFAULT_DBMS_ADDRESS = "127.0.0.1";
constexpr std::string_view DEFAULT_DBMS_DBNAME = "rms";
constexpr int DEFAULT_DBMS_PORT = 5432;
constexpr std::string_view DEFAULT_DBMS_USER = "rms_postgres";
constexpr std::string_view DEFAULT_DBMS_PASSWORD = "rms_defaultpassword";  // doesn't make much sense

class AccessorSettings : public ufa::settings::SettingsBase
{
public:
    SETTINGS_INIT(AccessorSettings)
    {
        SETTINGS_INIT_FIELD(dbmsAddress);
        SETTINGS_INIT_FIELD(dbmsDbname);
        SETTINGS_INIT_FIELD(dbmsPort);
        SETTINGS_INIT_FIELD(dbmsUser);
        SETTINGS_INIT_FIELD(dbmsPassword);
        SETTINGS_INIT_FIELD(connectAttempts);
    }

    SETTINGS_FIELD(dbmsAddress, std::string);
    SETTINGS_FIELD(dbmsDbname, std::string);
    SETTINGS_FIELD(dbmsPort, uint32_t);
    SETTINGS_FIELD(dbmsUser, std::string);
    SETTINGS_FIELD(dbmsPassword, std::string);
    SETTINGS_FIELD(connectAttempts, uint32_t);
};

struct IAccessor : public ufa::IBase
{
    virtual ufa::Result FillUser(data::User& user) = 0;

    virtual ufa::Result CreateRequirement(const db::data::User& initiativeUser, data::Requirement& req) = 0;
    virtual ufa::Result CreateSpecification(const db::data::User& initiativeUser,
        const data::Project& proj,
        data::Specification& spec) = 0;
    virtual ufa::Result CreateDependency(const db::data::User& initiativeUser, const data::Dependency& dep) = 0;
    virtual ufa::Result CreateProject(const db::data::User& initiativeUser, data::Project& proj) = 0;

    virtual ufa::Result AddReqInSpec(const db::data::User& initiativeUser,
        const data::Requirement& req,
        const data::Specification& spec,
        data::Section& sect) = 0;
    virtual ufa::Result AddUser(const db::data::User& initiativeUser, const data::User& user) = 0;

    virtual ufa::Result GetProjectInfo(const db::data::User& initiativeUser,
        data::Project& proj,
        std::vector<db::data::Specification>& specs,
        std::vector<db::data::Requirement>& reqs) = 0;
    virtual ufa::Result GetProjects(const db::data::User& initiativeUser, std::vector<db::data::Project>& projs) = 0;
    virtual ufa::Result GetDependencies(const db::data::User& initiativeUser,
        const db::data::Requirement& req,
        std::vector<db::data::Dependency>& deps) = 0;
    virtual ufa::Result GetRequirementsBySpecification(const db::data::User& initiativeUser,
        db::data::Specification& spec,
        std::vector<db::data::Requirement>& reqs) = 0;

    virtual ufa::Result EditRequirement(const db::data::User& initiativeUser,
        const db::data::Requirement& editReq,
        std::vector<db::data::Requirement>& impacted) = 0;

    virtual ufa::Result SendToApprove(const data::User& initiativeUser, const db::data::Specification& spec) = 0;
    virtual ufa::Result ApproveSpecification(const data::User& initiativeUser, const db::data::Approve& approve) = 0;
    virtual ufa::Result RejectSpecification(const data::User& initiativeUser, const db::data::Reject& reject) = 0;

    static std::unique_ptr<IAccessor> Create(std::shared_ptr<srv::IServiceLocator> locator);
};

}  // namespace db

#endif  // H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8