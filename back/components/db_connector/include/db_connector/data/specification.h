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
    enum class Status : uint64_t
    {
        Approved = 1,
        Draft = 2,
        Obsolete = 3
    };

    std::optional<uint64_t> id;
    std::optional<std::string> name;
    std::optional<uint64_t> analitycId;
    std::optional<Status> status;
    std::optional<uint64_t> projectId;
};

}  // namespace data
}  // namespace db

namespace string_converters
{

template <>
inline std::string ToString(::db::data::Specification::Status in)
{
    switch (in)
    {
        case ::db::data::Specification::Status::Approved:
            return "approved";
        case ::db::data::Specification::Status::Draft:
            return "draft";
        case ::db::data::Specification::Status::Obsolete:
            return "obsolete";
        default:
            return std::to_string(static_cast<int32_t>(in));
    }
}

template <>
inline ::db::data::Specification::Status FromString(const std::string& string)
{
    if (string == "approved")
        return ::db::data::Specification::Status::Approved;
    if (string == "draft")
        return ::db::data::Specification::Status::Draft;
    if (string == "obsolete")
        return ::db::data::Specification::Status::Obsolete;

    return static_cast<::db::data::Specification::Status>(-1);
}

}  // namespace string_converters

#endif  // H_68920800_B1BD_4CDB_AC42_9AE88B123CD2
