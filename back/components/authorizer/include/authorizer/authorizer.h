#ifndef H_02EC1EB2_180D_4B73_9660_9C68D0C4895B
#define H_02EC1EB2_180D_4B73_9660_9C68D0C4895B

#include <db_connector/accessor.h>
#include <db_connector/data/user.h>
#include <instrumental/common.h>

namespace auth
{

constexpr std::string_view USERID_PAYLOAD_KEY = "userId";  // seems to be used only in authorizer
constexpr std::string_view EXP_PAYLOAD_KEY = "expiration";
constexpr std::string_view ISSUER = "rms";
constexpr std::string_view SECRET = "secret";  // not so secret

struct IAuthorizer : public ufa::IBase
{
    /**
     * @brief check jwt token existence and validity
     * @param token input token
     * @param user output user data with filled id
     * @return ufa::Result SUCCESS on successful validation, WRONG_FORMAT when can't decode token, UNAUTHORIZED otherwise
     */
    virtual ufa::Result ValidateToken(std::string_view token, db::data::User& user) = 0;

    /**
     * @brief creates token with given user id
     * @param token output string containing a token
     * @param user must contain name and passwordHash
     * @return ufa::Result SUCCESS on success, UNAUTHORIZED otherwise
     */
    virtual ufa::Result GenerateToken(db::data::User& user, std::string& token) = 0;

    static std::unique_ptr<IAuthorizer> Create(std::shared_ptr<srv::IServiceLocator> locator, std::shared_ptr<db::IAccessor> accessor);
};

}  // namespace auth

#endif  // H_02EC1EB2_180D_4B73_9660_9C68D0C4895B