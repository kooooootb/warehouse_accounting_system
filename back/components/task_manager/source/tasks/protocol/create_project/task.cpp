#include <db_connector/data/requirement.h>
#include <db_connector/data/specification.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

CreateProject::CreateProject(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result CreateProject::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    auto accessor = depManager.GetAccessor();
    const auto getResult = accessor->CreateProject(m_initiativeUser, m_proj);

    if (getResult == ufa::Result::SUCCESS)
    {
        json json;
        json[ID_KEY] = m_proj.id.value();
        result = json.dump();
    }

    return getResult;
}

void CreateProject::ParseInternal(json&& json)
{
    m_proj.name = json.at(NAME_KEY).get<std::string>();
}

}  // namespace tasks
}  // namespace taskmgr
