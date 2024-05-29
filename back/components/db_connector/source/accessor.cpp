#include <sstream>

#include <db_connector/data/project.h>
#include <db_connector/data/requirement.h>
#include <db_connector/data/specification.h>
#include <db_connector/data/user.h>
#include <fmt/core.h>

#include <db_connector/accessor.h>
#include <db_connector/data/dependency.h>
#include <instrumental/common.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <settings_provider/settings_provider.h>
#include <tracer/tracer_provider.h>

#include "accessor.h"

#include "scripts/check_valid.h"
#include "scripts/initialize.h"

namespace db
{

constexpr std::string_view ADDRESS_CONN = "host";
constexpr std::string_view PORT_CONN = "port";
constexpr std::string_view DBNAME_CONN = "dbname";
constexpr std::string_view USER_CONN = "user";

Accessor::Accessor(std::shared_ptr<srv::IServiceLocator> locator) : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(locator->GetInterface(settingsProvider));

    AccessorSettings settings;
    settingsProvider->FillSettings(&settings);
    FillDefaultSettings(settings);
    AcceptSettings(std::move(settings));

    if (DBNeedsReinitializing())
    {
        InitializeDB();
    }
}

std::unique_ptr<IAccessor> IAccessor::Create(std::shared_ptr<srv::IServiceLocator> locator)
{
    return std::make_unique<Accessor>(std::move(locator));
}

void Accessor::FillDefaultSettings(AccessorSettings& settings)
{
    if (!settings.dbmsAddress.has_value())
        settings.dbmsAddress = DEFAULT_DBMS_ADDRESS;
    if (!settings.dbmsPort.has_value())
        settings.dbmsPort = DEFAULT_DBMS_PORT;
    if (!settings.dbmsUser.has_value())
        settings.dbmsUser = DEFAULT_DBMS_USER;
    if (!settings.dbmsDbname.has_value())
        settings.dbmsDbname = DEFAULT_DBMS_DBNAME;
}

void Accessor::AcceptSettings(AccessorSettings&& settings)
{
    CHECK_TRUE(settings.dbmsAddress.has_value());
    CHECK_TRUE(settings.dbmsDbname.has_value());
    CHECK_TRUE(settings.dbmsPort.has_value());
    CHECK_TRUE(settings.dbmsUser.has_value());

    std::stringstream connString;

    connString << ADDRESS_CONN << '=' << std::move(settings.dbmsAddress.value()) << ' ';
    connString << PORT_CONN << '=' << std::move(settings.dbmsPort.value()) << ' ';
    connString << DBNAME_CONN << '=' << std::move(settings.dbmsDbname.value()) << ' ';
    connString << USER_CONN << '=' << std::move(settings.dbmsUser.value());

    m_connOptions = connString.str();
}

bool Accessor::DBNeedsReinitializing()
{
    pqxx::connection conn(m_connOptions);
    pqxx::work work(conn);

    auto [count] = work.query1<int>(CHECK_VALID.data());
    work.commit();

    if (count != 1)
        return true;
    else
        return false;
}

void Accessor::InitializeDB()
{
    pqxx::connection conn(m_connOptions);
    pqxx::work work(conn);

    work.exec(INITIALIZE_DB);
    work.commit();
}

db::data::User::Role Accessor::GetRoleById(uint64_t id, pqxx::work& work)
{
    const auto query = fmt::format("SELECT per_role FROM Person WHERE person_id={}", id);
    const auto [roleInt] = work.query1<uint64_t>(query);
    return static_cast<db::data::User::Role>(roleInt);
}

ufa::Result Accessor::FillUser(data::User& user)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        std::stringstream query;
        query << "SELECT login, per_password, person_id FROM PERSON WHERE ";

        std::vector<std::string> filters;

        if (user.hashPassword.has_value())
            filters.emplace_back(std::string("per_password='").append(user.hashPassword.value()).append("'"));

        if (user.name.has_value())
            filters.emplace_back(std::string("login='").append(user.name.value()).append("'"));

        if (user.id.has_value())
            filters.emplace_back(std::string("person_id=").append(string_converters::ToString(user.id.value())).append(""));

        query << JoinFilters(filters);

        auto [login, perPassword, id] = work.query1<std::string, std::string, int>(query.str());
        work.commit();

        user.name = login;
        user.hashPassword = perPassword;
        user.id = id;

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::NOT_FOUND;
    }
}

ufa::Result Accessor::CreateRequirement(const db::data::User& initiativeUser, data::Requirement& req)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Analytic);

        const auto query = fmt::format(
            "INSERT INTO Requirement (type_id, description, status_req, project) VALUES ({}, '{}', 1, {}) RETURNING "
            "Requirement.requirement_id",
            static_cast<uint64_t>(req.type.value()),
            req.description.value(),
            req.projectId.value());

        const auto [reqId] = work.query1<uint64_t>(query);

        req.id = reqId;

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::CreateSpecification(const db::data::User& initiativeUser, const data::Project& proj, data::Specification& spec)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Analytic);

        const auto query = fmt::format(
            "INSERT INTO Specification (spec_name, analytic, status_spec, project) VALUES ('{}', {}, {}, {}) RETURNING "
            "Specification.specification_id",
            spec.name.value(),
            initiativeUser.id.value(),
            static_cast<uint64_t>(db::data::Specification::Status::Draft),
            proj.id.value());

        const auto [specId] = work.query1<uint64_t>(query);

        spec.id = specId;

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::CreateDependency(const db::data::User& initiativeUser, const data::Dependency& dep)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Analytic);

        const auto query = fmt::format("INSERT INTO relatedrequirements (main_req, depended_req, relation_type) VALUES ({}, {}, {})",
            dep.dependentId.value(),
            dep.dependencyId.value(),
            static_cast<uint64_t>(dep.type.value()));

        auto res = work.exec(query);
        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::CreateProject(const db::data::User& initiativeUser, data::Project& proj)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Analytic);

        const auto query =
            fmt::format("INSERT INTO project (project_name) VALUES ('{}') RETURNING project.project_id", proj.name.value());

        const auto [projId] = work.query1<uint64_t>(query);
        proj.id = projId;

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::AddReqInSpec(const db::data::User& initiativeUser,
    const data::Requirement& req,
    const data::Specification& spec,
    data::Section& sect)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Analytic);

        if (!sect.id.has_value())
        {
            // We need to create new section
            const auto sectionQuery =
                fmt::format("INSERT INTO section (sec_spec_num, SecName) VALUES ({}, '{}') RETURNING section.section_id",
                    sect.numInSpec.value(),
                    sect.name.value());

            sect.id = std::get<0>(work.query1<uint64_t>(sectionQuery));
        }

        const auto query = fmt::format(
            "INSERT INTO req_in_section_in_spec (spec_id, requirement_id, section_id, req_sec_num) VALUES ({}, {}, {}, {})",
            spec.id.value(),
            req.id.value(),
            sect.id.value(),
            req.numInSection.value());

        auto res = work.exec(query);

        const auto updateReqQuery = fmt::format("UPDATE Requirement SET status_req={} WHERE requirement_id={}",
            static_cast<uint64_t>(db::data::Requirement::Status::InSpecification),
            req.id.value());

        res = work.exec(updateReqQuery);

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::AddUser(const db::data::User& initiativeUser, const data::User& user)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Admin);

        const auto query = fmt::format(
            "INSERT INTO person (first_name, last_name, login, per_password, per_role) VALUES ('{}', '{}', '{}', '{}', {});",
            user.firstName.value(),
            user.lastName.value(),
            user.name.value(),
            user.hashPassword.value(),
            static_cast<uint64_t>(user.role.value()));

        auto res = work.exec(query);

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::GetProjectInfo(const db::data::User& initiativeUser,
    data::Project& proj,
    std::vector<db::data::Specification>& specs,
    std::vector<db::data::Requirement>& reqs)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        const auto query = fmt::format("SELECT project_name FROM Project WHERE project_id={};", proj.id.value());

        const auto [name] = work.query1<std::string>(query);
        proj.name = name;

        const auto specQuery = fmt::format("SELECT specification_id FROM Specification WHERE project={};", proj.id.value());

        for (const auto [specId] : work.query<uint64_t>(specQuery))
        {
            specs.emplace_back(db::data::Specification{.id = specId});
        }

        const auto reqQuery = fmt::format("SELECT requirement_id FROM Requirement WHERE project={};", proj.id.value());

        for (const auto [reqId] : work.query<uint64_t>(reqQuery))
        {
            reqs.emplace_back(db::data::Requirement{.id = reqId});
        }

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::GetProjects(const db::data::User& initiativeUser, std::vector<db::data::Project>& projs)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        const auto query = fmt::format("SELECT project_id FROM Project");

        for (const auto [projId] : work.query<uint64_t>(query))
        {
            projs.emplace_back(db::data::Project{.id = projId});
        }

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::GetDependencies(const db::data::User& initiativeUser,
    const db::data::Requirement& req,
    std::vector<db::data::Dependency>& deps)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Analytic);

        const auto query = fmt::format("SELECT depended_id, relation_type FROM RelatedRequirements WHERE main_req={}", req.id.value());

        for (const auto [depId, depType] : work.query<uint64_t, uint64_t>(query))
        {
            deps.emplace_back(db::data::Dependency{.dependencyId = depId, .type = static_cast<db::data::Dependency::Type>(depType)});
        }

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::GetRequirementsBySpecification(const db::data::User& initiativeUser,
    db::data::Specification& spec,
    std::vector<db::data::Requirement>& reqs)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        const auto specQuery =
            fmt::format("SELECT spec_name, analytic, status_spec FROM Specification WHERE specification_id={}", spec.id.value());

        const auto [specName, analytic, specStatus] = work.query1<std::string, uint64_t, uint64_t>(specQuery);

        spec.name = specName;
        spec.analitycId = analytic;
        spec.status = static_cast<db::data::Specification::Status>(specStatus);

        const auto reqQuery = fmt::format(
            "SELECT R.requirement_id, R.type_id, R.description, R.status_req FROM Requirement AS R LEFT JOIN Req_in_section_in_spec "
            "AS RSS ON R.requirement_id=RSS.requirement_id WHERE RSS.spec_id={}",
            spec.id.value());

        for (const auto [id, type, description, status] : work.query<uint64_t, uint64_t, std::string, uint64_t>(reqQuery))
        {
            reqs.emplace_back(db::data::Requirement{.id = id,
                .status = static_cast<db::data::Requirement::Status>(status),
                .type = static_cast<db::data::Requirement::Type>(type),
                .description = description});
        }

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::EditRequirement(const db::data::User& initiativeUser,
    const db::data::Requirement& editReq,
    std::vector<db::data::Requirement>& impacted)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        const auto query = fmt::format("UPDATE Requirement SET description='{}' WHERE requirement_id={}",
            editReq.description.value(),
            editReq.id.value());

        const auto res = work.exec(query);

        const auto depQuery = fmt::format("SELECT main_req FROM RelatedRequirements WHERE depended_req={} AND relation_type={}",
            editReq.id.value(),
            static_cast<uint64_t>(db::data::Dependency::Type::Hierarchy));

        for (const auto [depId] : work.query<uint64_t>(query))
        {
            impacted.emplace_back(db::data::Requirement{.id = depId});
        }

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::SendToApprove(const data::User& initiativeUser, const db::data::Specification& spec)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        const auto query = fmt::format(
            "UPDATE Requirement AS R SET status_req={} FROM Req_in_section_in_spec AS rss WHERE RSS.requirement_id=R.requirement_id "
            "AND RSS.spec_id={}",
            static_cast<uint64_t>(db::data::Requirement::Status::Negotiation),
            spec.id.value());

        const auto res = work.exec(query);

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::ApproveSpecification(const data::User& initiativeUser, const db::data::Approve& approve)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Reviewer);

        const auto rssQuery =
            fmt::format("SELECT RSS_id, spec_id FROM Req_in_Section_in_Spec WHERE requirement_id={}", approve.reqId.value());
        const auto [rssId, specId] = work.query1<uint64_t, uint64_t>(rssQuery);

        const auto addApproveQuery =
            fmt::format("INSERT INTO Approve (reviewer_id, rss_id, Ok) VALUES ({}, {}, true) RETURNING Approve.approve_id",
                initiativeUser.id.value(),
                rssId);
        const auto [approveId] = work.query1<uint64_t>(addApproveQuery);

        const auto addCommentQuery =
            fmt::format("INSERT INTO Comment (to_approve_id, decision_name) VALUES ({}, '{}')", approveId, approve.comment.value());
        const auto result = work.exec(addCommentQuery);

        // check and change specification status
        const auto reqsLeftToApprove = fmt::format(
            "SELECT Count(*) FROM Req_in_Section_in_Spec WHERE spec_id={} AND NOT EXISTS (SELECT * FROM Approve WHERE "
            "Approve.rss_id=Req_in_Section_in_Spec.RSS_id AND Approve.Ok=true)",
            specId);
        const auto [reqsLeft] = work.query1<uint64_t>(reqsLeftToApprove);

        const auto approveReq = fmt::format("UPDATE Requirement SET status_req={} WHERE requirement_id={}",
            static_cast<uint64_t>(db::data::Requirement::Status::Approved),
            specId);
        auto r = work.exec(approveReq);

        if (reqsLeft == 0)
        {
            const auto approveSpec = fmt::format("UPDATE Specification SET status_spec={} WHERE specification_id={}",
                static_cast<uint64_t>(db::data::Specification::Status::Approved),
                specId);
            const auto rssCount = work.exec(approveSpec);
        }

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result Accessor::RejectSpecification(const data::User& initiativeUser, const db::data::Reject& reject)
{
    try
    {
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        CHECK_TRUE(GetRoleById(initiativeUser.id.value(), work) == db::data::User::Role::Reviewer);

        const auto rssQuery =
            fmt::format("SELECT RSS_id, specification_id FROM Req_in_Section_in_Spec WHERE requirement_id={}", reject.reqId.value());
        const auto [rssId, specId] = work.query1<uint64_t, uint64_t>(rssQuery);

        const auto addRejectQuery =
            fmt::format("INSERT INTO Approve (reviewer_id, rss_id, Ok) VALUES ({}, {}, false) RETURNING Approve.approve_id",
                initiativeUser.id.value(),
                rssId);
        const auto [approveId] = work.query1<uint64_t>(addRejectQuery);

        const auto addCommentQuery =
            fmt::format("INSERT INTO Comment (to_approve_id, decision_name) VALUES ({}, '{}')", approveId, reject.comment.value());
        const auto result = work.exec(addCommentQuery);

        work.commit();

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while querying, message " << ex.what();
        return ufa::Result::ERROR;
    }
}

std::string Accessor::JoinFilters(const std::vector<std::string>& filters)
{
    std::stringstream result;
    for (auto it = filters.cbegin(), end = filters.cend() - 1; it != end; ++it)
    {
        result << *it << " AND ";
    }
    result << filters.back();

    return result.str();
}

}  // namespace db