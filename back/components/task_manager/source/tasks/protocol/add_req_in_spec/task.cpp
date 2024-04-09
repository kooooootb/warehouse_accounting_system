#include <db_connector/data/dependency.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

AddReqInSpec::AddReqInSpec(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result AddReqInSpec::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    result = json().dump();
    auto accessor = depManager.GetAccessor();

    return accessor->AddReqInSpec(m_initiativeUser, m_requirement, m_specification, m_section);
}

void AddReqInSpec::ParseInternal(json&& json)
{
    m_requirement.id = json.at(REQUIREMENT_ID_KEY).get<uint64_t>();
    m_specification.id = json.at(SPECIFICATION_ID_KEY).get<uint64_t>();
    m_requirement.numInSection = json.at(SECTION_REQNUM_KEY).get<uint64_t>();

    const auto jsonSection = json.at(SECTION_KEY);
    bool hasOne = false;

    if (jsonSection.contains(SECTION_ID_KEY))
    {
        m_section.id = jsonSection.at(SECTION_ID_KEY).get<uint64_t>();
        hasOne = true;
    }

    if (jsonSection.contains(SECTION_NAME_KEY))
    {
        m_section.name = jsonSection.at(SECTION_NAME_KEY).get<std::string>();
        m_section.numInSpec = jsonSection.at(SECTION_NUM_KEY).get<uint64_t>();
        hasOne = true;
    }

    CHECK_TRUE(hasOne);
}

}  // namespace tasks
}  // namespace taskmgr
