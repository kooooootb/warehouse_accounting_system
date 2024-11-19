#include <db_connector/query/query_factory.h>
#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tasks/common/warehouse_item.h>
#include <tracer/tracer.h>

#include <tasks/common/filter.h>
#include "get_warehouse_size.h"

namespace taskmgr
{
namespace tasks
{

GetWarehouseSize::GetWarehouseSize(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetWarehouseSize::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    int32_t count;

    const auto createResult = ActualGetWarehouseSize(*accessor, count);

    if (createResult == ufa::Result::SUCCESS)
    {
        jsonResult[COUNT_KEY] = std::move(count);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetWarehouseSize::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    const auto filtersIt = json.find(FILTERS_KEY);
    if (filtersIt != json.end())
    {
        std::shared_ptr<srv::IDateProvider> dateProvider;
        CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

        m_filter = ParseFilters(GetTracer(), *dateProvider, *filtersIt);
    }
}

ufa::Result GetWarehouseSize::ActualGetWarehouseSize(srv::IAccessor& accessor, int32_t& count)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    std::string queryString = "SELECT COUNT(*) FROM public.\"Warehouse\"";
    params_t params;

    if (m_filter != nullptr)
    {
        queryString += " WHERE ";

        placeholder_t placeholders;
        queryString += m_filter->ToString(placeholders);
        m_filter->CollectParams(params);
    }

    auto query = QueryFactory::CreateRaw(GetTracer(), queryString, std::move(params));

    result_t results;
    auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), m_filter == nullptr, &results);

    transaction->SetRootEntry(std::move(entry));

    auto transactionResult = transaction->Execute();

    if (transactionResult == ufa::Result::SUCCESS)
    {
        count = results.at(0, 0).get<int32_t>().value();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
