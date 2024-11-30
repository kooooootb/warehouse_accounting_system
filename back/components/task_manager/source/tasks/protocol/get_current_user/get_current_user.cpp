#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <date_provider/date_provider.h>
#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "get_current_user.h"

namespace taskmgr
{
namespace tasks
{

GetCurrentUser::GetCurrentUser(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetCurrentUser::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAuthorizer> authorizer;
    CHECK_SUCCESS(m_locator->GetInterface(authorizer));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = authorizer->GetUserInfo(m_initiativeUserId, m_user);

    if (createResult == ufa::Result::SUCCESS)
    {
        json jsonUser;

        util::json::Put(jsonUser, USER_ID_KEY, m_user.id.value());
        util::json::Put(jsonUser, NAME_KEY, m_user.name);
        util::json::Put(jsonUser, LOGIN_KEY, m_user.login);
        util::json::Put(jsonUser, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_user.created_date));
        util::json::Put(jsonUser, CREATED_BY_KEY, m_user.created_by);

        jsonResult[RESULT_KEY] = std::move(jsonUser);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetCurrentUser::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();
}

}  // namespace tasks
}  // namespace taskmgr
