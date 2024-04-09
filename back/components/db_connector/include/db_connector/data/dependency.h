#ifndef H_521F11B7_9480_445E_A32E_DAB3E0571D27
#define H_521F11B7_9480_445E_A32E_DAB3E0571D27

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct Dependency
{
    enum class Type : uint64_t
    {
        Hierarchy = 1,
        Horizontal = 2,
        Copied = 3,
    };

    std::optional<uint64_t> dependentId;
    std::optional<uint64_t> dependencyId;
    std::optional<Type> type;
};

}  // namespace data
}  // namespace db

#endif  // H_521F11B7_9480_445E_A32E_DAB3E0571D27