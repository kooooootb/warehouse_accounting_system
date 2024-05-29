#include <db_connector/data/requirement.h>
#include <db_connector/data/specification.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

GetProjects::GetProjects(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result GetProjects::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    std::vector<db::data::Project> projs;
    auto accessor = depManager.GetAccessor();

    const auto getResult = accessor->GetProjects(m_initiativeUser, projs);

    if (getResult == ufa::Result::SUCCESS)
    {
        std::vector<uint64_t> projIds;

        for (const auto& proj : projs)
            projIds.push_back(proj.id.value());

        json json;
        json[PROJECTIDS_KEY] = projIds;
        result = json.dump();
    }

    return getResult;
}

void GetProjects::ParseInternal(json&& json) {}

}  // namespace tasks
}  // namespace taskmgr
