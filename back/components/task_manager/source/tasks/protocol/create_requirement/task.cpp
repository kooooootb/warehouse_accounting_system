#include <db_connector/data/requirement.h>
#include <hash/hash.h>
#include <instrumental/types.h>
#include <sys/types.h>
#include <tracer/tracer.h>
#include <cstdint>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

CreateRequirement::CreateRequirement(std::shared_ptr<srv::ITracer> tracer, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(callback))
{
}

ufa::Result CreateRequirement::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    db::data::Requirement req;

    req.description = m_description;
    req.type = m_type;
    req.specificationId = m_specificationId;
    req.projectId = m_projectId;

    json jsonResult;

    auto accessor = depManager.GetAccessor();
    return accessor->CreateRequirement(req);
}

void CreateRequirement::ParseInternal(json&& json)
{
    m_description = json.at(DESCRIPTION_KEY).get<std::string>();
    m_type = json.at(TYPE_KEY).get<uint64_t>();
    m_specificationId = json.at(SPECIFICATION_ID_KEY).get<uint64_t>();
    m_projectId = json.at(PROJECT_ID_KEY).get<uint64_t>();
}

}  // namespace tasks
}  // namespace taskmgr
