#ifndef H_F85D0E1D_061C_42CC_BAC2_E13E4534D843
#define H_F85D0E1D_061C_42CC_BAC2_E13E4534D843

#include <string_view>

#include <instrumental/types.h>

#include <authorizer/authorizer.h>
#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace srv
{
namespace auth
{

class Authorizer : public srv::tracer::TracerProvider, public IAuthorizer
{
public:
    Authorizer(const std::shared_ptr<srv::IServiceLocator>& locator);

    ufa::Result ValidateToken(std::string_view token, userid_t& userId) override;
    ufa::Result GenerateToken(std::string_view login, std::string_view password, std::string& token, UserInfo& userInfo) override;

    ufa::Result CreateUser(auth::UserInfo& userInfo) override;
    ufa::Result GetUserInfo(userid_t userId, auth::UserInfo& result) override;

private:
    std::string GetSecretKey() const;
    ufa::Result ValidateCredentials(std::string_view login, std::string_view password, UserInfo& userInfo);

private:
    std::shared_ptr<srv::IAccessor> m_accessor;
    std::shared_ptr<srv::IDateProvider> m_dateProvider;
};

}  // namespace auth
}  // namespace srv

#endif  // H_F85D0E1D_061C_42CC_BAC2_E13E4534D843
