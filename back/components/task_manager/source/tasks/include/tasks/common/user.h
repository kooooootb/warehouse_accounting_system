#ifndef H_408076E9_CC95_44F5_BAA9_26BA37D6F08C
#define H_408076E9_CC95_44F5_BAA9_26BA37D6F08C

#include <optional>
#include <string>

#include <instrumental/time.h>
#include <instrumental/user.h>

namespace taskmgr
{
namespace tasks
{

struct User
{
    std::optional<int64_t> user_id;
    std::optional<std::string> login;
    std::optional<std::string> password;
    std::optional<std::string> name;
    std::optional<userid_t> created_by;
    std::optional<timestamp_t> created_date;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_408076E9_CC95_44F5_BAA9_26BA37D6F08C