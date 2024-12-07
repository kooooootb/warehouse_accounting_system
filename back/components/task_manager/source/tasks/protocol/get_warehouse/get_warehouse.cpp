#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tasks/common/warehouse_item.h>
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

    const auto createResult = ActualGetWarehouse(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json jsonWarehouse;

        util::json::Put(jsonWarehouse, WAREHOUSE_ID_KEY, m_warehouse.warehouse_id.value());
        util::json::Put(jsonWarehouse, NAME_KEY, m_warehouse.name.value());
        util::json::Put(jsonWarehouse, PRETTY_NAME_KEY, m_warehouse.pretty_name.value());
        util::json::Put(jsonWarehouse, DESCRIPTION_KEY, m_warehouse.description);
        util::json::Put(jsonWarehouse, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_warehouse.created_date.value()));
        util::json::Put(jsonWarehouse, CREATED_BY_KEY, m_warehouse.created_by.value());
        util::json::Put(jsonWarehouse, LOCATION_KEY, m_warehouse.location);

        json::array_t jsonItems;
        for (const auto& item : m_warehouseItems)
        {
            json jsonItem;

            util::json::Put(jsonItem, WAREHOUSE_ID_KEY, item.warehouse_id.value());
            util::json::Put(jsonItem, PRODUCT_ID_KEY, item.product_id.value());
            util::json::Put(jsonItem, COUNT_KEY, item.count.value());

            jsonItems.emplace_back(std::move(jsonItem));
        }

        jsonWarehouse[ITEMS_KEY] = std::move(jsonItems);
        jsonResult[RESULT_KEY] = std::move(jsonWarehouse);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetWarehouse::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_warehouse.warehouse_id = util::json::Get<int64_t>(json, ID_KEY);
}

ufa::Result GetWarehouse::ActualGetWarehouse(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Warehouse_Item;
    options->columns = {Column::product_id, Column::count};

    auto condition = CreateRealCondition(Column::warehouse_id, m_warehouse.warehouse_id.value());

    options->condition = std::move(condition);

    auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

    result_t itemResults;
    auto warehouseItemEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &itemResults);

    auto warehouseEntry = Warehouse::SelectEntry(GetTracer(), entriesFactory, m_warehouse);

    warehouseItemEntry->SetNext(std::move(warehouseEntry));

    transaction->SetRootEntry(std::move(warehouseItemEntry));

    auto transactionResult = transaction->Execute();

    if (transactionResult == ufa::Result::SUCCESS)
    {
        m_warehouseItems.reserve(itemResults.size());
        for (const auto& row : itemResults)
        {
            int i = 0;
            WarehouseItem item;

            item.warehouse_id = m_warehouse.warehouse_id.value();
            item.product_id = row.at(i++).get<int64_t>();
            item.count = row.at(i++).get<int64_t>();

            m_warehouseItems.emplace_back(std::move(item));
        }
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
