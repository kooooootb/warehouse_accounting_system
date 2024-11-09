#ifndef H_EAEC5B07_9D31_485C_9A05_AF97FB218C4D
#define H_EAEC5B07_9D31_485C_9A05_AF97FB218C4D

#include <optional>

#include <instrumental/common.h>

namespace srv
{
namespace auth
{

using userid_t = uint64_t;

/**
 * @brief this struct is only used when consumer needs full user data, in other cases fields usually passed separately
 * this is detailed mirror of User entity from db
 */
struct UserInfo
{
    /**
     * @brief unique db-generated id
     */
    userid_t id;

    /**
     * @brief unique human generated string
     */
    std::string login;

    /**
     * @brief password hashed with SHA256 and serialized to readable string with base64
     */
    std::string password_hashed;

    /**
     * @brief representative user generated name
     */
    std::string name;

    /**
     * @brief timestamp which shows when user was created
     */
    timestamp_t created_date;

    /**
     * @brief id of user who created this user, empty for superuser
     */
    std::optional<userid_t> created_by;
};

}  // namespace auth
}  // namespace srv

#endif  // H_EAEC5B07_9D31_485C_9A05_AF97FB218C4D