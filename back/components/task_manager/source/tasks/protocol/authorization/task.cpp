#include <hash/hash.h>

#include <authorizer/authorizer.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

Authorization::Authorization(std::shared_ptr<srv::ITracer> tracer, auth::userid_t userId, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(userId), std::move(callback))
{
}

ufa::Result Authorization::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{

    userData.name = m_username;
    userData.hashPassword = m_hashPassword;
    json jsonResult;

    std::string token;
    auto authorizer = depManager.GetAuthorizer();
    const auto authResult = authorizer->GenerateToken(userData, token);

    if (authResult == ufa::Result::SUCCESS)
    {
        jsonResult[TOKEN_KEY.data()] = token;
        result = jsonResult.dump();
    }

    return authResult;
}

void Authorization::ParseInternal(json&& json)
{
    m_username = json.at(USERNAME_KEY).get<std::string>();
    m_hashPassword = util::hash::HashToBase64(json.at(PASSWORD_KEY).get<std::string>());  // TODO
}

}  // namespace tasks
}  // namespace taskmgr
