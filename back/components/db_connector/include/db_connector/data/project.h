#ifndef H_C9A65EAB_E167_46B4_A93F_5324A47311FC
#define H_C9A65EAB_E167_46B4_A93F_5324A47311FC

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct Project
{
    std::optional<uint64_t> id;
    std::optional<std::string> name;
    std::optional<std::vector<uint64_t>> requirementIds;
    std::optional<std::vector<uint64_t>> specificationIds;
    std::optional<std::string> date;
};

}  // namespace data
}  // namespace db

#endif  // H_C9A65EAB_E167_46B4_A93F_5324A47311FC