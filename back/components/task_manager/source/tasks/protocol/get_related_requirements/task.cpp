#include <db_connector/data/dependency.h>
#include <db_connector/data/requirement.h>
#include <db_connector/data/specification.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

GetRelatedRequirements::GetRelatedRequirements(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result GetRelatedRequirements::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    std::vector<db::data::Dependency> deps;
    auto accessor = depManager.GetAccessor();

    const auto getResult = accessor->GetDependencies(m_initiativeUser, m_req, deps);

    if (getResult == ufa::Result::SUCCESS)
    {
        json jsonResult;
        jsonResult[DEPENDENCIES_KEY] = json::array();

        for (const auto& dep : deps)
        {
            json jsonDep = json::object();
            jsonDep[DEPENDENCYID_KEY] = dep.dependencyId.value();
            jsonDep[DEPENDENCYTYPE_KEY] = dep.type.value();
            jsonResult[DEPENDENCIES_KEY].emplace_back(std::move(jsonDep));
        }
        result = jsonResult.dump();
    }

    return getResult;
}

void GetRelatedRequirements::ParseInternal(json&& json)
{
    m_req.id = json.at(DEPENDENT_KEY).get<uint64_t>();
}

}  // namespace tasks
}  // namespace taskmgr
