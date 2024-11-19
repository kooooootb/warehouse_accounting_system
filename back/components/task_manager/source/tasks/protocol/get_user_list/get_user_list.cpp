#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tasks/common/product.h>
#include <tracer/tracer.h>

#include <tasks/common/filter.h>

#include "get_user_list.h"

namespace taskmgr
{
namespace tasks
{

GetUserList::GetUserList(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetUserList::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetUserList(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json::array_t jsonUsers;

        for (const auto& user : m_users)
        {
            json jsonUser;

            util::json::Put(jsonUser, USER_ID_KEY, user.user_id.value());
            util::json::Put(jsonUser, NAME_KEY, user.name.value());
            util::json::Put(jsonUser, LOGIN_KEY, user.name.value());
            util::json::Put(jsonUser, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(user.created_date.value()));
            util::json::Put(jsonUser, CREATED_BY_KEY, user.created_by);

            jsonUsers.emplace_back(jsonUser);
        }

        jsonResult[RESULT_KEY] = std::move(jsonUsers);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetUserList::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_limit = util::json::Get<int64_t>(json, LIMIT_KEY);
    m_offset = util::json::Get<int64_t>(json, OFFSET_KEY);

    const auto filtersIt = json.find(FILTERS_KEY);
    if (filtersIt != json.end())
    {
        std::shared_ptr<srv::IDateProvider> dateProvider;
        CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

        m_filter = ParseFilters(GetTracer(), *dateProvider, *filtersIt);
    }
}

ufa::Result GetUserList::ActualGetUserList(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::User;
    options->columns = {Column::user_id, Column::login, Column::name, Column::created_date, Column::created_by};

    options->orderBy = Column::user_id;
    options->limit = m_limit;
    options->offset = m_offset;

    if (m_filter != nullptr)
    {
        options->condition = std::move(m_filter);
    }

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
        m_users.reserve(results.size());

        for (const auto& row : results)
        {
            User user;
            int i = 0;

            user.user_id = row.at(i++).get<int64_t>();
            user.login = row.at(i++).get<std::string>();
            user.name = row.at(i++).get<std::string>();
            user.created_date = row.at(i++).get<timestamp_t>();
            user.created_by = row.at(i++).get<userid_t>();

            m_users.emplace_back(std::move(user));
        }
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
