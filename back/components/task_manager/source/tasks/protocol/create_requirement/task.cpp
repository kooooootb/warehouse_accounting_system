#include <db_connector/data/requirement.h>
#include <instrumental/types.h>
#include <tasks/base_task.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

CreateRequirement::CreateRequirement(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result CreateRequirement::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    json jsonResult;

    auto accessor = depManager.GetAccessor();
    const auto createResult = accessor->CreateRequirement(m_initiativeUser, m_req);

    if (createResult == ufa::Result::SUCCESS)
    {
        jsonResult[ID_KEY] = m_req.id.value();
        result = jsonResult.dump();
    }

    return createResult;
}

void CreateRequirement::ParseInternal(json&& json)
{
    m_req.description = json.at(DESCRIPTION_KEY).get<std::string>();
    m_req.type = static_cast<db::data::Requirement::Type>(json.at(TYPE_KEY).get<uint64_t>());
    m_req.projectId = json.at(PROJECT_ID_KEY).get<uint64_t>();
}

}  // namespace tasks
}  // namespace taskmgr
