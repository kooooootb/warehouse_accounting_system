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

#include "get_user.h"

namespace taskmgr
{
namespace tasks
{

GetUser::GetUser(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetUser::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetUser(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json jsonUser;

        util::json::Put(jsonUser, USER_ID_KEY, m_user.user_id.value());
        util::json::Put(jsonUser, NAME_KEY, m_user.name.value());
        util::json::Put(jsonUser, LOGIN_KEY, m_user.login.value());
        util::json::Put(jsonUser, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_user.created_date.value()));
        util::json::Put(jsonUser, CREATED_BY_KEY, m_user.created_by);

        jsonResult[RESULT_KEY] = std::move(jsonUser);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetUser::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_user.user_id = util::json::Get<int64_t>(json, ID_KEY);
}

ufa::Result GetUser::ActualGetUser(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::User;
    options->columns = {Column::login, Column::name, Column::created_date, Column::created_by};

    auto condition = CreateRealCondition(Column::user_id, m_user.user_id.value());

    options->condition = std::move(condition);

    auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

    result_t results;
    auto selectEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &results);

    transaction->SetRootEntry(std::move(selectEntry));

    auto transactionResult = transaction->Execute();

    if (results.empty())
    {
        transactionResult = ufa::Result::NOT_FOUND;
    }
    else if (transactionResult == ufa::Result::SUCCESS)
    {
        int i = 0;

        m_user.login = results.at(0, i++).get<std::string>();
        m_user.name = results.at(0, i++).get<std::string>();
        m_user.created_date = results.at(0, i++).get<timestamp_t>();
        m_user.created_by = results.at(0, i++).get<userid_t>();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
