#include <string_view>
#include <system_error>

#include <authorizer/authorizer.h>
#include <date_provider/date_provider.h>
#include <instrumental/check.h>
#include <instrumental/types.h>
#include <jwt-cpp/jwt.h>

#include <instrumental/string_converters.h>
#include <tracer/tracer_provider.h>

#include "authorizer.h"

namespace auth
{

Authorizer::Authorizer(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<db::IAccessor> accessor)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_accessor(std::move(accessor))
    , m_dateProvider(locator->GetInterface<srv::IDateProvider>())
{
}

std::unique_ptr<IAuthorizer> IAuthorizer::Create(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor)
{
    return std::make_unique<Authorizer>(std::move(locator), std::move(accessor));
}

ufa::Result Authorizer::ValidateToken(std::string_view token, db::data::User& user)
{
    try
    {
        auto verifier = jwt::verify().with_issuer(ISSUER.data()).allow_algorithm(jwt::algorithm::hs256{GetSecretKey()});
        std::string tokenStr = std::string(token);  // decode takes string
        auto decoded = jwt::decode(tokenStr);

        auto payload = decoded.get_payload_json();

        const auto expirationDate = payload[EXP_PAYLOAD_KEY.data()];
        if (m_dateProvider->GetTimestamp() > string_converters::FromString<srv::IDateProvider::timestamp_t>(expirationDate.to_str()))
        {
            // expired
            TRACE_ERR << TRACE_HEADER << "Received expired token: " << token;
            return ufa::Result::UNAUTHORIZED;
        }

        user.id = string_converters::FromString<uint64_t>(payload[USERID_PAYLOAD_KEY.data()].to_str());

        std::error_code ec;
        verifier.verify(decoded, ec);
        if (ec)
        {
            TRACE_ERR << TRACE_HEADER << "Verification failure for token: " << token << ", error message: " << ec.message();
            return ufa::Result::UNAUTHORIZED;
        }
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Invalid token: " << token;
        return ufa::Result::WRONG_FORMAT;
    }

    return ufa::Result::SUCCESS;
}

ufa::Result Authorizer::GenerateToken(db::data::User& user, std::string& token)
{
    CHECK_TRUE(!user.name->empty());
    CHECK_TRUE(!user.hashPassword->empty());

    const auto res = m_accessor->FillUser(user);
    if (res == ufa::Result::NOT_FOUND)
    {
        TRACE_ERR << TRACE_HEADER << "User not found: " << user.name.value();
        return ufa::Result::UNAUTHORIZED;
    }
    CHECK_SUCCESS(res);
    CHECK_TRUE(user.id.has_value());

    token = jwt::create()
                .set_issuer(ISSUER.data())
                .set_payload_claim(USERID_PAYLOAD_KEY.data(), jwt::claim(string_converters::ToString(user.id.value())))
                .set_payload_claim(EXP_PAYLOAD_KEY.data(),
                    jwt::claim(string_converters::ToString(
                        m_dateProvider->GetTimestamp() + 30ull * 60ull * 60ull * 1000ull * 1000ull * 1000ull)))
                .sign(jwt::algorithm::hs256{GetSecretKey()});

    return ufa::Result::SUCCESS;
}

std::string Authorizer::GetSecretKey() const
{
    return "secret";
}

}  // namespace auth
