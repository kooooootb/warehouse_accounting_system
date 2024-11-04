#ifndef H_62CF423F_FAC7_4287_AD99_44BE377C166A
#define H_62CF423F_FAC7_4287_AD99_44BE377C166A

#include <fmt/core.h>

#include <string>

namespace srv
{
namespace db
{
namespace dbmgr
{
namespace scpts
{

inline std::string CreateDatabase(std::string_view dbname)
{
    using namespace std::literals;
    return fmt::format("CREATE DATABASE {};"sv, dbname);
}

}  // namespace scpts
}  // namespace dbmgr
}  // namespace db
}  // namespace srv

#endif  // H_62CF423F_FAC7_4287_AD99_44BE377C166A
