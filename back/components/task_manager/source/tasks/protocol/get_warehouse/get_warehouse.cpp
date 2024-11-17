#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "get_warehouse.h"

namespace taskmgr
{
namespace tasks
{

GetWarehouse::GetWarehouse(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetWarehouse::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetWarehouse(*accessor, *dateProvider);

    if (createResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, WAREHOUSE_ID_KEY, m_warehouse.warehouse_id.value());
        util::json::Put(jsonResult, NAME_KEY, m_warehouse.name.value());
        util::json::Put(jsonResult, PRETTY_NAME_KEY, m_warehouse.pretty_name.value());
        util::json::Put(jsonResult, DESCRIPTION_KEY, m_warehouse.description);
        util::json::Put(jsonResult, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_warehouse.created_date.value()));
        util::json::Put(jsonResult, CREATED_BY_KEY, m_warehouse.created_by.value());
    }

    result = jsonResult.dump();
    return createResult;
}

void GetWarehouse::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_warehouse.warehouse_id = util::json::Get<int64_t>(json, ID_KEY);
    m_warehouse.created_by = m_initiativeUserId;
}

ufa::Result GetWarehouse::ActualGetWarehouse(srv::IAccessor& accessor, srv::IDateProvider& dateProvider)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Warehouse;
    options->columns = {Column::name, Column::pretty_name, Column::description, Column::created_date, Column::created_by};

    auto condition = CreateRealCondition(Column::warehouse_id, m_warehouse.warehouse_id.value());

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

        m_warehouse.name = results.at(0, i++).get<std::string>();
        m_warehouse.pretty_name = results.at(0, i++).get<std::string>();
        m_warehouse.description = results.at(0, i++).get<std::string>();
        m_warehouse.created_date = results.at(0, i++).get<timestamp_t>();
        m_warehouse.created_by = results.at(0, i++).get<userid_t>();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
