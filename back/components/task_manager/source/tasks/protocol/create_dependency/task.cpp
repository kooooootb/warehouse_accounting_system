#include <db_connector/data/dependency.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

CreateDependency::CreateDependency(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result CreateDependency::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    result = json().dump();
    auto accessor = depManager.GetAccessor();

    for (const auto& dep : m_deps)
    {
        if (const auto stepResult = accessor->CreateDependency(m_initiativeUser, dep); stepResult != ufa::Result::SUCCESS)
        {
            return stepResult;
        }
    }

    return ufa::Result::SUCCESS;
}

void CreateDependency::ParseInternal(json&& json)
{
    const auto dependentId = json.at(REQUIREMENT_ID_KEY).get<uint64_t>();

    for (const auto& dependency : json.at(DEPENDENCIES_IDS_KEY).items())
    {
        db::data::Dependency dep;
        dep.dependentId = dependentId;
        dep.dependencyId = dependency.value().at(DEPENDENCY_ID_KEY).get<uint64_t>();
        dep.type = static_cast<db::data::Dependency::Type>(dependency.value().at(DEPENDENCY_TYPE_KEY).get<uint64_t>());

        m_deps.emplace_back(std::move(dep));
    }
}

}  // namespace tasks
}  // namespace taskmgr
