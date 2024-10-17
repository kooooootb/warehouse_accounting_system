#include <string_view>
#include <system_error>

#include <jwt-cpp/jwt.h>

#include <authorizer/authorizer.h>
#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <instrumental/check.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <tracer/tracer_provider.h>

#include "authorizer.h"

namespace srv
{

namespace auth
{

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

ufa::Result Authorizer::GenerateToken(std::string_view login, std::string_view password, std::string& token)
{
    TRACE_INF << TRACE_HEADER;

    try
    {
        CHECK_TRUE(!login.empty());
        CHECK_TRUE(!password.empty());

        const auto res = ufa::Result::SUCCESS;  //= m_accessor->FillUser(user);  // wrong, todo fix after accessor improvements
        if (res == ufa::Result::NOT_FOUND)
        {
            TRACE_ERR << TRACE_HEADER << "User not found: " << login;
            return ufa::Result::UNAUTHORIZED;
        }
        CHECK_SUCCESS(res);

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

}  // namespace auth
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IAuthorizer, srv::auth::Authorizer);
