#include <hash/hash.h>

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

Authorization::Authorization(std::shared_ptr<srv::ITracer> tracer, srv::auth::userid_t userId, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(userId), std::move(callback))
{
}

ufa::Result Authorization::ExecuteInternal(const srv::IServiceLocator& locator, std::string& result)
{
    json jsonResult;

    std::shared_ptr<srv::IAuthorizer> authorizer;
    CHECK_SUCCESS(locator.GetInterface(authorizer));

    std::string token;
    const auto authResult = authorizer->GenerateToken(m_login, m_hashPassword, token);

    if (authResult == ufa::Result::SUCCESS)
    {
        jsonResult[TOKEN_KEY.data()] = token;
        result = jsonResult.dump();
    }

    return authResult;
}

void Authorization::ParseInternal(json&& json)
{
    m_login = json.at(USERNAME_KEY).get<std::string>();
    m_hashPassword = util::hash::HashToBase64(json.at(PASSWORD_KEY).get<std::string>());  // TODO
}

}  // namespace tasks
}  // namespace taskmgr
