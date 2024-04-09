#ifndef H_C83D8D62_2051_49D3_866B_D7B80092A1B6
#define H_C83D8D62_2051_49D3_866B_D7B80092A1B6

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct Approve
{
    std::optional<std::string> comment;
    std::optional<uint64_t> reqId;
};

}  // namespace data
}  // namespace db

#endif  // H_C83D8D62_2051_49D3_866B_D7B80092A1B6
