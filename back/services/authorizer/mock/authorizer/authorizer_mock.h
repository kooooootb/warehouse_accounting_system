#ifndef H_E14C67DD_F525_4F57_87C9_0280F1DA8C7C
#define H_E14C67DD_F525_4F57_87C9_0280F1DA8C7C

#include <gmock/gmock.h>

#include <authorizer/authorizer.h>

namespace srv
{
namespace mock
{

struct AuthorizerMock : public IAuthorizer
{
    MOCK_METHOD(ufa::Result, ValidateToken, (std::string_view token, userid_t& userId), (override));
    MOCK_METHOD(ufa::Result,
        GenerateToken,
        (std::string_view login, std::string_view password, std::string& token, UserInfo& userInfo),
        (override));
    MOCK_METHOD(ufa::Result, CreateUser, (auth::UserInfo & userInfo), (override));
    MOCK_METHOD(ufa::Result, GetUserInfo, (userid_t userId, auth::UserInfo& result), (override));

    static std::shared_ptr<AuthorizerMock> Create()
    {
        using namespace testing;

        auto authorizer = std::make_shared<srv::mock::AuthorizerMock>();

        return authorizer;
    }
};

}  // namespace mock
}  // namespace srv

#endif  // H_E14C67DD_F525_4F57_87C9_0280F1DA8C7C