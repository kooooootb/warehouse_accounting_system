#ifndef H_02EC1EB2_180D_4B73_9660_9C68D0C4895B
#define H_02EC1EB2_180D_4B73_9660_9C68D0C4895B

#include <instrumental/common.h>
#include <locator/service.h>
#include <locator/service_locator.h>

namespace srv
{
namespace auth
{

using userid_t = uint64_t;

/**
 * @brief this struct is only used when consumer needs full user data, in other cases fields are passed separately
 * @todo extend me
 */
struct UserInfo
{
    /**
     * @brief unique machine generated id
     */
    userid_t id;

    /**
     * @brief unique human readable string
     */
    std::string login;
};

}  // namespace auth

struct IAuthorizer : public srv::IService
{
    DECLARE_IID(0X5EF435B7);

    /**
     * @brief check jwt token existence and validity
     * @param token input token
     * @param userId output user id
     * @return ufa::Result SUCCESS on successful validation, WRONG_FORMAT when can't decode token, UNAUTHORIZED otherwise
     */
    virtual ufa::Result ValidateToken(std::string_view token, auth::userid_t& userId) = 0;

    /**
     * @brief creates token for user with given login
     * @param login given login
     * @param password password for user with given login
     * @param token output string containing a token
     * @todo make secure password
     * @return ufa::Result SUCCESS on success, UNAUTHORIZED otherwise
     */
    virtual ufa::Result GenerateToken(std::string_view login, std::string_view password, std::string& token) = 0;
};

}  // namespace srv

#endif  // H_02EC1EB2_180D_4B73_9660_9C68D0C4895B