#ifndef H_821A8A0C_C4F0_46B3_80EB_31B45CCCDCB5
#define H_821A8A0C_C4F0_46B3_80EB_31B45CCCDCB5

#include <string_view>

namespace db
{

constexpr std::string_view CHECK_VALID = R"(
SELECT COUNT(relname) FROM pg_class WHERE relname='person'
)";

}  // namespace db

#endif  // H_821A8A0C_C4F0_46B3_80EB_31B45CCCDCB5