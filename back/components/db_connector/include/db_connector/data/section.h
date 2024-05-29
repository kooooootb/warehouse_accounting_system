#ifndef H_F344B056_5609_4805_A06A_A9B3F56B8A56
#define H_F344B056_5609_4805_A06A_A9B3F56B8A56

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct Section
{
    std::optional<uint64_t> id;
    std::optional<std::string> name;
    std::optional<uint64_t> numInSpec;
};

}  // namespace data
}  // namespace db

#endif  // H_F344B056_5609_4805_A06A_A9B3F56B8A56