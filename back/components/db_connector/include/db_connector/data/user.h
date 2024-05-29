#ifndef H_3EF37A28_A113_49D1_A912_BAB8040BE0A9
#define H_3EF37A28_A113_49D1_A912_BAB8040BE0A9

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct User
{
    enum Role : uint64_t
    {
        Admin = 1,
        Analytic = 2,
        Reviewer = 3,
    };

    std::optional<uint64_t> id;
    std::optional<std::string> name;
    std::optional<std::string> hashPassword;
    std::optional<std::string> firstName;
    std::optional<std::string> lastName;
    std::optional<Role> role;
};

}  // namespace data
}  // namespace db

namespace string_converters
{

template <>
inline std::string ToString(::db::data::User::Role in)
{
    switch (in)
    {
        case ::db::data::User::Admin:
            return "admin";
        case ::db::data::User::Analytic:
            return "analytic";
        case ::db::data::User::Reviewer:
            return "reviewer";
        default:
            return std::to_string(static_cast<int32_t>(in));
    }
}

template <>
inline ::db::data::User::Role FromString(const std::string& string)
{
    if (string == "admin")
        return ::db::data::User::Role::Admin;
    if (string == "Analytic")
        return ::db::data::User::Role::Analytic;
    if (string == "Reviewer")
        return ::db::data::User::Role::Reviewer;

    return static_cast<::db::data::User::Role>(-1);
}

}  // namespace string_converters

#endif  // H_3EF37A28_A113_49D1_A912_BAB8040BE0A9