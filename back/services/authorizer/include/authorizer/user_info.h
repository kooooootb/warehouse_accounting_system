#ifndef H_EAEC5B07_9D31_485C_9A05_AF97FB218C4D
#define H_EAEC5B07_9D31_485C_9A05_AF97FB218C4D

#include <fmt/core.h>

#include <optional>

#include <instrumental/common.h>
#include <instrumental/string_converters.h>

namespace srv
{
namespace auth
{

/**
 * @brief this struct is only used when consumer needs full user data, in other cases fields usually passed separately
 * this is detailed mirror of User entity from db
 */
struct UserInfo
{
    /**
     * @brief unique db-generated id, optional as usually used when id is not known yet
     */
    std::optional<userid_t> id;

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
    timestamp_t created_date = 0;

    /**
     * @brief id of user who created this user, empty for superuser
     */
    std::optional<userid_t> created_by;
};

}  // namespace auth
}  // namespace srv

namespace string_converters
{

template <>
inline std::string ToString(::srv::auth::UserInfo info)
{
    using namespace std::literals;

    std::string idString = info.id.has_value() ? string_converters::ToString(info.id.value()) : std::string("empty"sv);
    std::string createdByString =
        info.created_by.has_value() ? string_converters::ToString(info.created_by.value()) : std::string("empty"sv);

    return fmt::format("<id: [{}], login: [{}], name: [{}], created_date: [{}], created_by: [{}]>"sv,
        idString,
        info.login,
        info.name,
        string_converters::ToString(info.created_date),
        createdByString);
}

}  // namespace string_converters

#endif  // H_EAEC5B07_9D31_485C_9A05_AF97FB218C4D