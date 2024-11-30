#include <string_view>
#include <system_error>

#include <jwt-cpp/jwt.h>

#include <instrumental/check.h>
#include <instrumental/string_converters.h>
#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/product_definitions/columns.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/insert_query_params.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>
#include <db_connector/query/utilities.h>
#include <tracer/trace_macros.h>
#include <tracer/tracer_provider.h>

#include "authorizer.h"

namespace srv
{

namespace auth
{

constexpr std::string_view USERID_PAYLOAD_KEY = "userId";
constexpr std::string_view EXP_PAYLOAD_KEY = "expiration";
constexpr std::string_view ISSUER = "was";
constexpr std::string_view SECRET = "secret";  // not so secret, todo

Authorizer::Authorizer(const std::shared_ptr<srv::IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_accessor(locator->GetInterface<srv::IAccessor>())
    , m_dateProvider(locator->GetInterface<srv::IDateProvider>())
{
    TRACE_DBG << TRACE_HEADER;
}

ufa::Result Authorizer::ValidateToken(std::string_view token, userid_t& userId)
{
    TRACE_DBG << TRACE_HEADER;

    try
    {
        auto verifier = jwt::verify().with_issuer(ISSUER.data()).allow_algorithm(jwt::algorithm::hs256{GetSecretKey()});
        auto decoded = jwt::decode(std::string(token));

        auto payload = decoded.get_payload_json();

        const auto expirationDate = payload[EXP_PAYLOAD_KEY.data()];
        if (m_dateProvider->GetTimestamp() > string_converters::FromString<timestamp_t>(expirationDate.to_str()))
        {
            // expired
            TRACE_ERR << TRACE_HEADER << "Received expired token: " << token;
            return ufa::Result::UNAUTHORIZED;
        }

        userId = string_converters::FromString<uint64_t>(payload[USERID_PAYLOAD_KEY.data()].to_str());

        std::error_code ec;
        verifier.verify(decoded, ec);
        if (ec)
        {
            TRACE_ERR << TRACE_HEADER << "Verification failure for token: " << token << ", error message: " << ec.message();
            return ufa::Result::UNAUTHORIZED;
        }

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Invalid token: " << token;
        return ufa::Result::WRONG_FORMAT;
    }
}

ufa::Result Authorizer::GenerateToken(std::string_view login, std::string_view password, std::string& token, UserInfo& userInfo)
{
    TRACE_INF << TRACE_HEADER << "login: " << login;

    try
    {
        CHECK_TRUE(!login.empty());
        CHECK_TRUE(!password.empty());

        const auto res = ValidateCredentials(login, password, userInfo);

        if (res != ufa::Result::SUCCESS)
        {
            TRACE_ERR << TRACE_HEADER << "User not found: " << login << ", validation error: " << res;
            return ufa::Result::UNAUTHORIZED;
        }

        token = jwt::create()
                    .set_issuer(ISSUER.data())
                    .set_payload_claim(USERID_PAYLOAD_KEY.data(), jwt::claim(string_converters::ToString(userInfo.id.value())))
                    .set_payload_claim(EXP_PAYLOAD_KEY.data(),
                        jwt::claim(string_converters::ToString(
                            m_dateProvider->GetTimestamp() + 30ull * 60ull * 60ull * 1000ull * 1000ull * 1000ull)))
                    .sign(jwt::algorithm::hs256{GetSecretKey()});

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << ex.what();
        return ufa::Result::UNAUTHORIZED;
    }
}

ufa::Result Authorizer::CreateUser(auth::UserInfo& userInfo)
{
    TRACE_INF << TRACE_HEADER << "User info: " << userInfo;

    userInfo.created_date = m_dateProvider->GetTimestamp();

    // checking necessary fields
    if (userInfo.login.empty() || userInfo.name.empty() || userInfo.password_hashed.empty() || !userInfo.created_by.has_value())
    {
        return ufa::Result::WRONG_FORMAT;
    }

    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(m_accessor->CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<InsertOptions>();
    InsertValues values;

    options->table = Table::User;
    options->columns = {Column::name, Column::password_hashed, Column::login, Column::created_date, Column::created_by};
    options->returning = {Column::user_id};

    params_t insertingValues;
    insertingValues.Append(userInfo.name)
        .Append(userInfo.password_hashed)
        .Append(userInfo.login)
        .Append(userInfo.created_date)
        .Append(userInfo.created_by.value());
    values.values.emplace_back(std::move(insertingValues));

    auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

    result_t result;
    auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &result);

    transaction->SetRootEntry(std::move(entry));

    const auto transactionResult = transaction->Execute();

    if (transactionResult == ufa::Result::SUCCESS)
    {
        userInfo.id = result.at(0, 0).get<int64_t>();
    }

    return transactionResult;
}

ufa::Result Authorizer::GetUserInfo(userid_t userId, auth::UserInfo& userInfo)
{
    TRACE_INF << TRACE_HEADER << "User id: " << userId;

    try
    {
        using namespace srv::db;

        std::unique_ptr<ITransaction> transaction;
        CHECK_SUCCESS(m_accessor->CreateTransaction(transaction));

        auto& entriesFactory = transaction->GetEntriesFactory();

        auto options = std::make_unique<SelectOptions>();
        SelectValues values;

        options->table = Table::User;
        options->columns = {Column::login, Column::name, Column::created_by, Column::created_date};

        auto condition = CreateRealCondition(Column::user_id, userId);

        options->condition = std::move(condition);

        auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

        result_t result;
        auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &result);

        transaction->SetRootEntry(std::move(entry));

        transaction->Execute();

        if (result.empty())
        {
            return ufa::Result::NOT_FOUND;
        }
        else if (result.size() == 1)
        {
            CHECK_TRUE(result.columns() == 4);

            const auto& row = result.at(0);

            userInfo.id = userId;

            userInfo.login = row[0].as<std::string>();
            userInfo.name = row[1].as<std::string>();
            userInfo.created_by = row[2].get<userid_t>();
            userInfo.created_date = row[2].as<timestamp_t>();
        }
        else
        {
            CHECK_SUCCESS(ufa::Result::ERROR, "sane check");
        }
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Received exception while validating, what(): " << ex.what();
        return ufa::Result::ERROR;
    }

    return ufa::Result::SUCCESS;
}

std::string Authorizer::GetSecretKey() const
{
    return std::string(SECRET);
}

ufa::Result Authorizer::ValidateCredentials(std::string_view login, std::string_view password, UserInfo& userInfo)
{
    try
    {
        using namespace srv::db;

        std::unique_ptr<ITransaction> transaction;
        CHECK_SUCCESS(m_accessor->CreateTransaction(transaction));

        auto& entriesFactory = transaction->GetEntriesFactory();

        auto options = std::make_unique<SelectOptions>();
        SelectValues values;

        options->table = Table::User;
        options->columns = {Column::user_id, Column::name, Column::created_by, Column::created_date};

        auto loginCondition = CreateRealCondition(Column::login, login);
        auto passwordCondition = CreateRealCondition(Column::password_hashed, password);

        auto grouped = CreateGroupCondition(GroupConditionType::AND);
        grouped->conditions.emplace_back(std::move(loginCondition));
        grouped->conditions.emplace_back(std::move(passwordCondition));

        options->condition = std::move(grouped);

        auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

        result_t result;
        auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &result);

        transaction->SetRootEntry(std::move(entry));

        transaction->Execute();

        if (result.empty())
        {
            return ufa::Result::NOT_FOUND;
        }
        else if (result.size() == 1)
        {
            int i = 0;

            userInfo.id = result.at(0, i++).as<userid_t>();
            userInfo.name = result.at(0, i++).as<std::string>();
            userInfo.created_by = result.at(0, i++).get<userid_t>();
            userInfo.created_date = result.at(0, i++).as<userid_t>();
            userInfo.login = login;
        }
        else
        {
            CHECK_SUCCESS(ufa::Result::WRONG_FORMAT, "Got several users with same login and password");
        }
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Received exception while validating, what(): " << ex.what();
        return ufa::Result::ERROR;
    }

    return ufa::Result::SUCCESS;
}

}  // namespace auth
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IAuthorizer, srv::auth::Authorizer);
