#ifndef H_68920800_B1BD_4CDB_AC42_9AE88B123CD2
#define H_68920800_B1BD_4CDB_AC42_9AE88B123CD2

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct Specification
{
    std::optional<uint64_t> id;
    std::optional<std::string> name;
    std::optional<uint64_t> analitycId;
};

}  // namespace data
}  // namespace db

#endif  // H_68920800_B1BD_4CDB_AC42_9AE88B123CD2
