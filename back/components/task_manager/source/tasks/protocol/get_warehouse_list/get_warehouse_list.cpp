#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tasks/common/invoice_item.h>
#include <tasks/common/warehouse.h>
#include <tasks/common/warehouse_item.h>
#include <tracer/tracer.h>

#include <tasks/common/filter.h>

#include "get_warehouse_list.h"

namespace taskmgr
{
namespace tasks
{

GetWarehouseList::GetWarehouseList(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetWarehouseList::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetWarehouseList(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json::array_t jsonWarehouses;

        for (const auto& warehouse : m_warehouses)
        {
            json jsonWarehouse;

            util::json::Put(jsonWarehouse, WAREHOUSE_ID_KEY, warehouse.warehouse_id.value());
            util::json::Put(jsonWarehouse, NAME_KEY, warehouse.name.value());
            util::json::Put(jsonWarehouse, PRETTY_NAME_KEY, warehouse.pretty_name.value());
            util::json::Put(jsonWarehouse, DESCRIPTION_KEY, warehouse.description);
            util::json::Put(jsonWarehouse, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(warehouse.created_date.value()));
            util::json::Put(jsonWarehouse, CREATED_BY_KEY, warehouse.created_by.value());

            json::array_t jsonItems;
            for (const auto& item : m_warehouseItems[warehouse.warehouse_id.value()])
            {
                json jsonItem;

                util::json::Put(jsonItem, WAREHOUSE_ID_KEY, item.warehouse_id.value());
                util::json::Put(jsonItem, PRODUCT_ID_KEY, item.product_id.value());
                util::json::Put(jsonItem, COUNT_KEY, item.count.value());

                jsonItems.emplace_back(std::move(jsonItem));
            }
            jsonWarehouse[ITEMS_KEY] = std::move(jsonItems);

            jsonWarehouses.emplace_back(std::move(jsonWarehouse));
        }

        jsonResult[RESULT_KEY] = std::move(jsonWarehouses);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetWarehouseList::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_limit = util::json::GetOptional<int64_t>(json, LIMIT_KEY);
    m_limit = m_limit.has_value() && m_limit.value() > 0 ? m_limit : std::nullopt;

    m_offset = util::json::Get<int64_t>(json, OFFSET_KEY);

    const auto filtersIt = json.find(FILTERS_KEY);
    if (filtersIt != json.end())
    {
        std::shared_ptr<srv::IDateProvider> dateProvider;
        CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

        m_filter = ParseFilters(GetTracer(), *dateProvider, *filtersIt);
    }
}

ufa::Result GetWarehouseList::ActualGetWarehouseList(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Warehouse;
    options->columns =
        {Column::warehouse_id, Column::name, Column::pretty_name, Column::description, Column::created_date, Column::created_by};

    options->orderBy = Column::warehouse_id;

    if (m_limit.has_value())
    {
        options->limit = m_limit.value();
    }

    options->offset = m_offset;

    if (m_filter != nullptr)
    {
        options->condition = std::move(m_filter);
    }

    auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

    result_t warehouseResults;
    auto selectEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &warehouseResults);

    result_t warehouseItemResults;
    const auto function = [&warehouseResults, &entriesFactory, &warehouseItemResults, this]() -> void
    {
        this->m_warehouses.reserve(warehouseResults.size());

        for (const auto& row : warehouseResults)
        {
            Warehouse warehouse;
            int i = 0;

            warehouse.warehouse_id = row.at(i++).get<int64_t>().value();
            warehouse.name = row.at(i++).get<std::string>().value();
            warehouse.pretty_name = row.at(i++).get<std::string>().value();
            warehouse.description = row.at(i++).get<std::string>().value();
            warehouse.created_date = row.at(i++).get<timestamp_t>().value();
            warehouse.created_by = row.at(i++).get<userid_t>().value();

            this->m_warehouses.emplace_back(std::move(warehouse));
        }

        std::vector<int64_t> warehousesIds;
        warehousesIds.reserve(this->m_warehouses.size());

        for (const auto& warehouse : this->m_warehouses)
        {
            warehousesIds.push_back(warehouse.warehouse_id.value());
        }

        if (!warehousesIds.empty())
        {
            auto options = std::make_unique<SelectOptions>();
            SelectValues values;

            options->table = Table::Warehouse_Item;
            options->columns = {Column::warehouse_id, Column::product_id, Column::count};

            options->orderBy = Column::warehouse_id;

            auto condition = CreateInCondition(Column::warehouse_id, warehousesIds);
            options->condition = std::move(condition);

            auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

            auto warehouseItemEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &warehouseItemResults);
            warehouseItemEntry->Execute();
        }
    };

    auto converterEntry = entriesFactory.CreateVariableTransactionEntry(std::move(function));

    selectEntry->SetNext(std::move(converterEntry));
    transaction->SetRootEntry(std::move(selectEntry));

    auto transactionResult = transaction->Execute();

    if (warehouseResults.empty())
    {
        transactionResult = ufa::Result::NOT_FOUND;
    }
    else if (transactionResult == ufa::Result::SUCCESS)
    {
        for (const auto& row : warehouseItemResults)
        {
            WarehouseItem item;
            int i = 0;

            item.warehouse_id = row.at(i++).get<int64_t>();
            item.product_id = row.at(i++).get<int64_t>();
            item.count = row.at(i++).get<int64_t>();

            m_warehouseItems[item.warehouse_id.value()].emplace_back(std::move(item));
        }
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
