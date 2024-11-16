#include <hash/hash.h>
#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <date_provider/date_provider.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "create_user.h"

namespace taskmgr
{
namespace tasks
{

CreateUser::CreateUser(std::shared_ptr<srv::ITracer> tracer, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(taskInfo))
{
}

ufa::Result CreateUser::ExecuteInternal(const srv::IServiceLocator& locator, std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAuthorizer> authorizer;
    CHECK_SUCCESS(locator.GetInterface(authorizer));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(locator.GetInterface(dateProvider));

    const auto createResult = authorizer->CreateUser(m_userInfo);

    if (createResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, NAME_KEY, m_userInfo.name);
        util::json::Put(jsonResult, LOGIN_KEY, m_userInfo.login);
        util::json::Put(jsonResult, PASSWORD_KEY, m_userInfo.password_hashed);
        util::json::Put(jsonResult, USER_ID_KEY, m_userInfo.id.value());
        util::json::Put(jsonResult, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_userInfo.created_date));
        util::json::Put(jsonResult, CREATED_BY_KEY, m_userInfo.created_by.value());
    }

    result = jsonResult.dump();

    return createResult;
}

void CreateUser::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_userInfo.login = util::json::Get<std::string>(json, LOGIN_KEY);
    m_userInfo.name = util::json::Get<std::string>(json, NAME_KEY);
    m_userInfo.password_hashed = util::hash::HashToBase64(util::json::Get<std::string>(json, PASSWORD_KEY));
    m_userInfo.created_by = m_initiativeUserId;
}

}  // namespace tasks
}  // namespace taskmgr
