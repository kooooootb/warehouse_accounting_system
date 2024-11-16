#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "create_warehouse.h"

namespace taskmgr
{
namespace tasks
{

CreateWarehouse::CreateWarehouse(std::shared_ptr<srv::ITracer> tracer, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(taskInfo))
{
}

ufa::Result CreateWarehouse::ExecuteInternal(const srv::IServiceLocator& locator, std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(locator.GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(locator.GetInterface(dateProvider));

    const auto createResult = ActualCreateWarehouse(*accessor, *dateProvider);

    if (createResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, NAME_KEY, m_warehouse.name.value());
        util::json::Put(jsonResult, DESCRIPTION_KEY, m_warehouse.description);
        util::json::Put(jsonResult, LOCATION_KEY, m_warehouse.location);
        util::json::Put(jsonResult, WAREHOUSE_ID_KEY, m_warehouse.warehouse_id.value());
        util::json::Put(jsonResult, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_warehouse.created_date.value()));
        util::json::Put(jsonResult, CREATED_BY_KEY, m_warehouse.created_by.value());
    }

    result = jsonResult.dump();
    return createResult;
}

void CreateWarehouse::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_warehouse.name = util::json::Get<std::string>(json, NAME_KEY);
    m_warehouse.description = util::json::GetOptional<std::string>(json, DESCRIPTION_KEY);
    m_warehouse.location = util::json::GetOptional<std::string>(json, LOCATION_KEY);
    m_warehouse.created_by = m_initiativeUserId;
}

ufa::Result CreateWarehouse::ActualCreateWarehouse(srv::IAccessor& accessor, srv::IDateProvider& dateProvider)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    result_t insertionResults;

    auto insertEntry = Warehouse::InsertEntry(GetTracer(), entriesFactory, insertionResults, m_warehouse, dateProvider);

    transaction->SetRootEntry(std::move(insertEntry));

    const auto transactionResult = transaction->Execute();

    if (transactionResult == ufa::Result::SUCCESS)
    {
        m_warehouse.warehouse_id = insertionResults.at(0, 0).get<int64_t>();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
