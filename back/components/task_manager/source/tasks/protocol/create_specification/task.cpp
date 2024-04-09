#include <db_connector/data/requirement.h>
#include <instrumental/types.h>
#include <tasks/base_task.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

CreateSpecification::CreateSpecification(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result CreateSpecification::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    json jsonResult;
    auto accessor = depManager.GetAccessor();

    const auto createResult = accessor->CreateSpecification(m_initiativeUser, m_proj, m_spec);

    if (createResult == ufa::Result::SUCCESS)
    {
        jsonResult[ID_KEY] = m_spec.id.value();
        result = jsonResult.dump();
    }

    return createResult;
}

void CreateSpecification::ParseInternal(json&& json)
{
    m_proj.id = json.at(PROJECTID_KEY).get<uint64_t>();
    m_spec.name = json.at(NAME_KEY).get<std::string>();
}

}  // namespace tasks
}  // namespace taskmgr
