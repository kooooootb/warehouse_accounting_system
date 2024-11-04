#ifndef H_F85D0E1D_061C_42CC_BAC2_E13E4534D843
#define H_F85D0E1D_061C_42CC_BAC2_E13E4534D843

#include <authorizer/authorizer.h>
#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace srv
{
namespace auth
{

constexpr std::string_view USERID_PAYLOAD_KEY = "userId";
constexpr std::string_view EXP_PAYLOAD_KEY = "expiration";
constexpr std::string_view ISSUER = "was";
constexpr std::string_view SECRET = "secret";  // not so secret, todo

class Authorizer : public srv::tracer::TracerProvider, public IAuthorizer
{
public:
    Authorizer(const std::shared_ptr<srv::IServiceLocator>& locator);

    ufa::Result ValidateToken(std::string_view token, userid_t& userId) override;
    ufa::Result GenerateToken(std::string_view login, std::string_view password, std::string& token) override;

private:
    std::string GetSecretKey() const;

private:
    std::shared_ptr<srv::IAccessor> m_accessor;
    std::shared_ptr<srv::IDateProvider> m_dateProvider;
};

}  // namespace auth
}  // namespace srv

#endif  // H_F85D0E1D_061C_42CC_BAC2_E13E4534D843
