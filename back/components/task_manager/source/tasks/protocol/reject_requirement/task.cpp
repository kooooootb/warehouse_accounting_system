#include <db_connector/data/requirement.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

RejectSpecification::RejectSpecification(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result RejectSpecification::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    json jsonResult;

    auto accessor = depManager.GetAccessor();
    return accessor->RejectSpecification(m_initiativeUser, m_approve);
}

void RejectSpecification::ParseInternal(json&& json)
{
    m_approve.reqId = json.at(ID_KEY).get<uint64_t>();

    if (json.contains(COMMENT_KEY))
        m_approve.comment = json.at(COMMENT_KEY).get<std::string>();
}

}  // namespace tasks
}  // namespace taskmgr
