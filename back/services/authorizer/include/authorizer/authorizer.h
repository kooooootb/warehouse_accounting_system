#ifndef H_02EC1EB2_180D_4B73_9660_9C68D0C4895B
#define H_02EC1EB2_180D_4B73_9660_9C68D0C4895B

#include <instrumental/common.h>

#include <locator/service.h>
#include <locator/service_locator.h>

namespace srv
{

struct IAuthorizer : public srv::IService
{
    DECLARE_IID(0X5EF435B7);

    /**
     * @brief check jwt token existence and validity, executes quick as all data is in token
     * @param token input token
     * @param userId output user id
     * @return ufa::Result SUCCESS on successful validation, WRONG_FORMAT when can't decode token, UNAUTHORIZED otherwise
     */
    virtual ufa::Result ValidateToken(std::string_view token, userid_t& userId) = 0;

    /**
     * @brief creates token for user with given login, executes slow as we need to validate credentials in db
     * @param login given login
     * @param password password for user with given login
     * @param token output string containing a token
     * @param userId output containing found user_id
     * @todo make secure password
     * @return ufa::Result SUCCESS on success, UNAUTHORIZED otherwise
     */
    virtual ufa::Result GenerateToken(std::string_view login, std::string_view password, std::string& token, userid_t& userid) = 0;
};

}  // namespace srv

#endif  // H_02EC1EB2_180D_4B73_9660_9C68D0C4895B