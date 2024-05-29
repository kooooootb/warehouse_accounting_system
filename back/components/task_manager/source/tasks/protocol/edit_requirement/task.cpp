#include <db_connector/data/requirement.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

EditRequirement::EditRequirement(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result EditRequirement::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    json jsonResult;

    std::vector<db::data::Requirement> impactedReqs;

    auto accessor = depManager.GetAccessor();
    const auto editResult = accessor->EditRequirement(m_initiativeUser, m_editReq, impactedReqs);

    if (editResult == ufa::Result::SUCCESS)
    {
        auto impJson = json::array();
        for (const auto& req : impactedReqs)
        {
            impJson.emplace_back(req.id.value());
        }

        jsonResult[IMPACTED_KEY] = json::array();
        result = jsonResult.dump();
    }

    return editResult;
}

void EditRequirement::ParseInternal(json&& json)
{
    m_editReq.id = json.at(ID_KEY).get<uint64_t>();
    m_editReq.description = json.at(DESCRIPTION_KEY).get<std::string>();
}

}  // namespace tasks
}  // namespace taskmgr
