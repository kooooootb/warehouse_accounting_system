#ifndef H_2E85A573_5552_4637_BA16_B33869A4DC52
#define H_2E85A573_5552_4637_BA16_B33869A4DC52

#include <optional>

#include <instrumental/common.h>

namespace db
{
namespace data
{

struct Requirement
{
    enum class Status : uint64_t
    {
        New = 1,
        InSpecification = 2,
        Negotiation = 3,
        Approved = 4,
        Developed = 5,
        Supported = 6,
        Deleted = 7
    };

    enum class Type : uint64_t
    {
        Functional = 1,
        Nonfunctional = 2
    };

    std::optional<uint64_t> id;
    std::optional<Status> status;
    std::optional<Type> type;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<uint64_t> specificationId;
    std::optional<std::string> release;
    std::optional<uint64_t> projectId;
    std::optional<uint64_t> numInSection;
};

}  // namespace data
}  // namespace db

namespace string_converters
{

template <>
inline std::string ToString(::db::data::Requirement::Status in)
{
    switch (in)
    {
        case ::db::data::Requirement::Status::New:
            return "new";
        case ::db::data::Requirement::Status::InSpecification:
            return "inspecification";
        case ::db::data::Requirement::Status::Negotiation:
            return "negotiation";
        case ::db::data::Requirement::Status::Approved:
            return "approved";
        case ::db::data::Requirement::Status::Developed:
            return "developed";
        case ::db::data::Requirement::Status::Supported:
            return "supported";
        case ::db::data::Requirement::Status::Deleted:
            return "deleted";
        default:
            return std::to_string(static_cast<int32_t>(in));
    }
}

template <>
inline ::db::data::Requirement::Status FromString(const std::string& string)
{
    if (string == "new")
        return ::db::data::Requirement::Status::New;
    if (string == "inspecification")
        return ::db::data::Requirement::Status::InSpecification;
    if (string == "negotiation")
        return ::db::data::Requirement::Status::Negotiation;
    if (string == "approved")
        return ::db::data::Requirement::Status::Approved;
    if (string == "developed")
        return ::db::data::Requirement::Status::Developed;
    if (string == "supported")
        return ::db::data::Requirement::Status::Supported;
    if (string == "deleted")
        return ::db::data::Requirement::Status::Deleted;

    return static_cast<::db::data::Requirement::Status>(-1);
}

template <>
inline std::string ToString(::db::data::Requirement::Type in)
{
    switch (in)
    {
        case ::db::data::Requirement::Type::Functional:
            return "functional";
        case ::db::data::Requirement::Type::Nonfunctional:
            return "nonfunctional";
        default:
            return std::to_string(static_cast<int32_t>(in));
    }
}

template <>
inline ::db::data::Requirement::Type FromString(const std::string& string)
{
    if (string == "functional")
        return ::db::data::Requirement::Type::Functional;
    if (string == "nonfunctional")
        return ::db::data::Requirement::Type::Nonfunctional;

    return static_cast<::db::data::Requirement::Type>(-1);
}

}  // namespace string_converters

#endif  // H_2E85A573_5552_4637_BA16_B33869A4DC52
