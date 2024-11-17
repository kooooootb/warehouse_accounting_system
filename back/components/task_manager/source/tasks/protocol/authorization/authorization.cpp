#include <hash/hash.h>
#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "authorization.h"

namespace taskmgr
{
namespace tasks
{

Authorization::Authorization(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
    TRACE_INF << TRACE_HEADER;
}

ufa::Result Authorization::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAuthorizer> authorizer;
    CHECK_SUCCESS(m_locator->GetInterface(authorizer));

    std::string token;
    userid_t userid;
    const auto authResult = authorizer->GenerateToken(m_login, m_hashPassword, token, userid);

    if (authResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, TOKEN_KEY, token);
        util::json::Put(jsonResult, USERID_KEY, userid);
    }

    result = jsonResult.dump();
    return authResult;
}

void Authorization::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_login = util::json::Get<std::string>(json, USERNAME_KEY);
    m_hashPassword = util::hash::HashToBase64(util::json::Get<std::string>(json, PASSWORD_KEY));
}

}  // namespace tasks
}  // namespace taskmgr
