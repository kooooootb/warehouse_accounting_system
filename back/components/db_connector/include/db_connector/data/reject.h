#ifndef H_E54B516B_AB06_4450_B2E2_9C32055E9B76
#define H_E54B516B_AB06_4450_B2E2_9C32055E9B76

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct Reject
{
    std::optional<std::string> comment;
    std::optional<uint64_t> reqId;
};

}  // namespace data
}  // namespace db

#endif  // H_E54B516B_AB06_4450_B2E2_9C32055E9B76