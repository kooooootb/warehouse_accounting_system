#include <hash/hash.h>
#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <date_provider/date_provider.h>
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
    TRACE_DBG << TRACE_HEADER;
}

ufa::Result Authorization::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAuthorizer> authorizer;
    CHECK_SUCCESS(m_locator->GetInterface(authorizer));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    std::string token;
    srv::auth::UserInfo userInfo;
    const auto authResult = authorizer->GenerateToken(m_login, m_hashPassword, token, userInfo);

    if (authResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, TOKEN_KEY, token);
        util::json::Put(jsonResult, USERID_KEY, userInfo.id);
        util::json::Put(jsonResult, LOGIN_KEY, userInfo.login);
        util::json::Put(jsonResult, NAME_KEY, userInfo.name);
        util::json::Put(jsonResult, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(userInfo.created_date));
        util::json::Put(jsonResult, CREATED_BY_KEY, userInfo.created_by);
    }

    result = jsonResult.dump();
    return authResult;
}

void Authorization::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_login = util::json::Get<std::string>(json, LOGIN_KEY);
    m_hashPassword = util::hash::HashToBase64(util::json::Get<std::string>(json, PASSWORD_KEY));
}

}  // namespace tasks
}  // namespace taskmgr
