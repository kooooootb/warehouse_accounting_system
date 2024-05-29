#include <db_connector/data/requirement.h>
#include <db_connector/data/specification.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

GetProjectInfo::GetProjectInfo(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result GetProjectInfo::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    std::vector<db::data::Requirement> reqs;
    std::vector<db::data::Specification> specs;
    auto accessor = depManager.GetAccessor();

    const auto getResult = accessor->GetProjectInfo(m_initiativeUser, m_proj, specs, reqs);

    if (getResult == ufa::Result::SUCCESS)
    {
        std::vector<uint64_t> reqIds, specIds;  // can't be bothered thinking more

        for (const auto& req : reqs)
            reqIds.push_back(req.id.value());
        for (const auto& spec : specs)
            specIds.push_back(spec.id.value());

        json json;
        json[REQUIREMENTIDS_KEY] = reqIds;
        json[SPECIFICATIONIDS_KEY] = specIds;
        json[NAME_KEY] = m_proj.name.value();
        result = json.dump();
    }

    return getResult;
}

void GetProjectInfo::ParseInternal(json&& json)
{
    m_proj.id = json.at(PROJECTID_KEY).get<uint64_t>();
}

}  // namespace tasks
}  // namespace taskmgr
