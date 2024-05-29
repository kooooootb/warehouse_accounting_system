#include <db_connector/data/requirement.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

SendSpecToApprove::SendSpecToApprove(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result SendSpecToApprove::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    json jsonResult;

    auto accessor = depManager.GetAccessor();
    return accessor->SendToApprove(m_initiativeUser, m_spec);
}

void SendSpecToApprove::ParseInternal(json&& json)
{
    m_spec.id = json.at(ID_KEY).get<uint64_t>();
}

}  // namespace tasks
}  // namespace taskmgr
