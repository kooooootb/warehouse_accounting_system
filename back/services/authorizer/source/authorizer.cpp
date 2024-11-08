#include <string_view>
#include <system_error>

#include <jwt-cpp/jwt.h>

#include <instrumental/check.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>

#include <authorizer/authorizer.h>
#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/query/condition.h>
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
    TRACE_INF << TRACE_HEADER;
}

ufa::Result Authorizer::ValidateToken(std::string_view token, userid_t& userId)
{
    TRACE_INF << TRACE_HEADER;

    try
    {
        auto verifier = jwt::verify().with_issuer(ISSUER.data()).allow_algorithm(jwt::algorithm::hs256{GetSecretKey()});
        auto decoded = jwt::decode(std::string(token));

        auto payload = decoded.get_payload_json();

        const auto expirationDate = payload[EXP_PAYLOAD_KEY.data()];
        if (m_dateProvider->GetTimestamp() > string_converters::FromString<srv::IDateProvider::timestamp_t>(expirationDate.to_str()))
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

ufa::Result Authorizer::GenerateToken(std::string_view login, std::string_view password, std::string& token, auth::userid_t& userid)
{
    TRACE_INF << TRACE_HEADER;

    try
    {
        CHECK_TRUE(!login.empty());
        CHECK_TRUE(!password.empty());

        // const auto res = ufa::Result::SUCCESS;  //= m_accessor->FillUser(user);  // wrong, todo fix after accessor improvements
        const auto res = ValidateCredentials(login, password, userid);

        if (res != ufa::Result::SUCCESS)
        {
            TRACE_ERR << TRACE_HEADER << "User not found: " << login << ", validation error: " << res;
            return ufa::Result::UNAUTHORIZED;
        }

        token = jwt::create()
                    .set_issuer(ISSUER.data())
                    .set_payload_claim(USERID_PAYLOAD_KEY.data(), jwt::claim(std::string(login)))
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

std::string Authorizer::GetSecretKey() const
{
    return std::string(SECRET);
}

ufa::Result Authorizer::ValidateCredentials(std::string_view login, std::string_view password, userid_t& userid)
{
    try
    {
        using namespace srv::db;

        std::unique_ptr<db::ITransaction> transaction;
        CHECK_SUCCESS(m_accessor->CreateTransaction(transaction));

        auto& entriesFactory = transaction->GetEntriesFactory();

        auto options = std::make_unique<SelectOptions>();
        auto values = std::make_unique<SelectValues>();

        options->table = Table::User;
        options->columns = {Column::user_id};

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
            CHECK_TRUE(result.columns() == 1);
            userid = result.at(0, 0).as<userid_t>();
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
