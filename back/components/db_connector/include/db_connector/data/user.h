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
    std::optional<uint64_t> id;
    std::optional<std::string> name;
    std::optional<std::string> hashPassword;
};

}  // namespace data
}  // namespace db

#endif  // H_3EF37A28_A113_49D1_A912_BAB8040BE0A9