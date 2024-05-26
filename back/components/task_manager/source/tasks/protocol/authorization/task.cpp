#include <db_connector/data/user.h>
#include <hash/hash.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

Authorization::Authorization(std::shared_ptr<srv::ITracer> tracer, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(callback))
{
}

ufa::Result Authorization::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    db::data::User userData;
    userData.name = m_username;
    userData.hashPassword = m_hashPassword;

    auto authorizer = depManager.GetAuthorizer();
    return authorizer->GenerateToken(userData, result);
}

void Authorization::ParseInternal(json&& json)
{
    m_username = json[USERNAME_KEY].get<std::string>();
    m_hashPassword = util::hash::HashString(json[PASSWORD_KEY].get<std::string>());
}

}  // namespace tasks
}  // namespace taskmgr
