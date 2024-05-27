#include <sstream>

#include <pqxx/pqxx>

#include <db_connector/accessor.h>
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

ufa::Result Accessor::FillUser(data::User& user)
{
    try
    {
        CHECK_TRUE(user.hashPassword.has_value() || user.id.has_value() || user.name.has_value());
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

ufa::Result Accessor::CreateRequirement(data::Requirement& req)
{
    try
    {
        CHECK_TRUE(req.status.has_value() || req.projectId.has_value() || req.specificationId.has_value() || req.type.has_value() ||
                   req.description.has_value());
        pqxx::connection conn(m_connOptions);
        pqxx::work work(conn);

        std::stringstream query;
        query << "INSERT INTO requirement (type_id, in_specification, description, status_req, project) VALUES (";

        query << req.type.value() << ", " << req.specificationId.value() << ", '" << req.description.value() << "', "
              << 1 /*status*/ << ", " << req.projectId.value();

        query << ")";

        auto res = work.exec(query.str());
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