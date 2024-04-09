#ifndef H_F85D0E1D_061C_42CC_BAC2_E13E4534D843
#define H_F85D0E1D_061C_42CC_BAC2_E13E4534D843

#include <authorizer/authorizer.h>
#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace auth
{

class Authorizer : public srv::tracer::TracerProvider, public IAuthorizer
{
public:
    Authorizer(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<db::IAccessor> accessor);

    ufa::Result ValidateToken(std::string_view token, db::data::User& user) override;
    ufa::Result GenerateToken(db::data::User& user, std::string& token) override;

private:
    std::string GetSecretKey() const;

private:
    std::shared_ptr<db::IAccessor> m_accessor;
    std::shared_ptr<srv::IDateProvider> m_dateProvider;
};

}  // namespace auth

#endif  // H_F85D0E1D_061C_42CC_BAC2_E13E4534D843
