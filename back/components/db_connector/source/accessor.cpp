#include <chrono>
#include <exception>
#include <shared_mutex>
#include <sstream>
#include <thread>

#include <db_connector/data/project.h>
#include <db_connector/data/requirement.h>
#include <db_connector/data/specification.h>
#include <db_connector/data/user.h>
#include <fmt/core.h>

#include <db_connector/accessor.h>
#include <db_connector/data/dependency.h>
#include <instrumental/check.h>
#include <instrumental/common.h>
#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <settings_provider/settings_provider.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>

#include "accessor.h"

#include "scripts/check_valid.h"
#include "scripts/initialize.h"

namespace db
{

constexpr std::string_view DEFAULT_ADDRESS = "127.0.0.1";
constexpr uint32_t DEFAULT_PORT = 5432;
constexpr std::string_view DEFAULT_DBNAME = "postgres";
constexpr std::string_view DEFAULT_USER = "postgres";
constexpr std::string_view DEFAULT_PASSWORD = "defaultpassword";  // doesn't make much sense
constexpr uint32_t DEFAULT_CONNECTION_ATTEMPTS = 50;

ConnectionOptions::ConnectionOptions(std::shared_ptr<srv::ITracer> tracer)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_address(DEFAULT_ADDRESS)
    , m_port(DEFAULT_PORT)
    , m_dbname(DEFAULT_DBNAME)
    , m_user(DEFAULT_USER)
    , m_password(DEFAULT_PASSWORD)
{
}

void ConnectionOptions::SetSettings(const AccessorSettings& settings)
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
        connString << ADDRESS_CONN << '=' << std::move(m_address) << ' ';
        connString << DBNAME_CONN << '=' << std::move(m_dbname) << ' ';
        connString << PORT_CONN << '=' << std::move(m_port) << ' ';
        connString << USER_CONN << '=' << std::move(m_user) << ' ';
        connString << PASSWORD_CONN << '=' << std::move(m_password);

        m_cachedString = connString.str();

        // lock dies here so return in this block
        return m_cachedString;
    }

    return m_cachedString;
}

Accessor::Accessor(std::shared_ptr<srv::IServiceLocator> locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_connectionOptions(GetTracer())
{
    TRACE_INF << TRACE_HEADER;

    std::shared_ptr<srv::ISettingsProvider> settingsProvider;
    CHECK_SUCCESS(locator->GetInterface(settingsProvider));

    AccessorSettings settings;
    settingsProvider->FillSettings(&settings);

    SetSettings(std::move(settings));

    if (DBNeedsReinitializing())
    {
        InitializeDB();
    }
}

std::unique_ptr<IAccessor> IAccessor::Create(std::shared_ptr<srv::IServiceLocator> locator)
{
    return std::make_unique<Accessor>(std::move(locator));
}

void Accessor::SetSettings(const AccessorSettings& settings)
{
    m_connectionOptions.SetSettings(settings);

    {
        std::shared_lock lock(m_optionsMutex);

        ufa::TryExtractFromOptional(settings.connectAttempts, m_connectAttempts);
        ufa::TryExtractFromOptional(settings.alwaysReinitialize, m_alwaysReinitialize);
    }
}

bool Accessor::DBNeedsReinitializing()
{
    {
        std::shared_lock lock(m_optionsMutex);
        if (m_alwaysReinitialize)
        {
            return true;
        }
    }

    auto conn = CreateConnection();
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
    auto conn = CreateConnection();
    pqxx::work work(conn);

    work.exec(INITIALIZE_DB);
    work.commit();
}

pqxx::connection Accessor::CreateConnection()
{
    constexpr std::chrono::duration Timeout = std::chrono::seconds(2);

    uint32_t attempt = 0;

    do
    {
        try
        {
            return pqxx::connection(m_connectionOptions.GetConnectionString());
        }
        catch (const std::exception& ex)
        {
            TRACE_WRN << TRACE_HEADER << "Failed connecting to db, attempt: " << attempt;
            std::this_thread::sleep_for(Timeout);
        }
    } while (++attempt <= m_connectAttempts);

    CHECK_SUCCESS(ufa::Result::NO_CONNECTION,
        "Connection to db failed after " << attempt << " retries, connection string: " << m_connectionOptions.GetConnectionString());
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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
        auto conn = CreateConnection();
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