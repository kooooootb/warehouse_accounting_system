#ifndef H_C9453D73_17F3_4A71_9370_943429690DFD
#define H_C9453D73_17F3_4A71_9370_943429690DFD

#include <optional>
#include <string>

#include <instrumental/time.h>
#include <instrumental/user.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/product_definitions/columns.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/delete_query_params.h>
#include <db_connector/query/insert_query_params.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>
#include <db_connector/query/update_query_params.h>
#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/query_transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry_factory.h>
#include <tracer/tracer.h>

#include "helpers.h"
#include "product.h"

namespace taskmgr
{
namespace tasks
{

struct WarehouseItem
{
    std::optional<int64_t> warehouse_id;
    std::optional<int64_t> product_id;
    std::optional<int64_t> count;

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> InsertsEntry(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        const std::vector<Product>& products,
        int64_t warehouseId)
    {
        using namespace srv::db;

        const auto function =
            [tracer = std::move(tracer), &entriesFactory = entriesFactory, &products = products, warehouseId = warehouseId]() -> void
        {
            LOCAL_TRACER(tracer);

            auto options = std::make_unique<InsertOptions>();
            InsertValues values;

            options->table = Table::Warehouse_Item;
            options->columns = {Column::warehouse_id, Column::count, Column::product_id};
            options->returning = {Column::product_id};

            for (const auto& product : products)
            {
                params_t params;

                params.Append(warehouseId).Append(product.count.value()).Append(product.id.value());

                values.values.emplace_back(std::move(params));
            }

            auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

            result_t results;
            auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), false, &results);
            entry->Execute();
        };

        return entriesFactory.CreateVariableTransactionEntry(std::move(function));
    }

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> ChangeCount(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        std::vector<Product>& products,
        int64_t warehouseId,
        bool isReducing)
    {
        const auto function = [tracer = std::move(tracer), &entriesFactory, &products, warehouseId, isReducing]() -> void
        {
            using namespace srv::db;

            LOCAL_TRACER(tracer);

            if (products.empty())
            {
                return;
            }

            std::vector<int64_t> toDeleteIds;     // or toCreateIds
            std::vector<int64_t> toCreateCounts;  // only for inserts
            std::vector<std::pair<int64_t /* product_id */, int64_t /* target */>> toModifyIdsTarget;

            const auto predicate = isReducing ? [](Product& product, const result_t& result){
                return product.count >= result.at(0, 0).as<int64_t>();
            } : [](Product& product, const result_t& result){
                return result.empty();
            };

            const auto calcTarget = isReducing ? [](Product& product, const result_t& result){
                return result.at(0, 0).as<int64_t>() - product.count.value() ;
            } : [](Product& product, const result_t& result){
                return result.at(0, 0).as<int64_t>() + product.count.value() ;
            };

            for (size_t i = 0; i < products.size(); ++i)
            {
                CHECK_TRUE(products[i].id.has_value());
                CHECK_TRUE(products[i].count.has_value());

                auto options = std::make_unique<SelectOptions>();
                SelectValues values;

                options->table = Table::Warehouse_Item;
                options->columns = {Column::count};

                auto productCondition = CreateRealCondition(Column::product_id, products[i].id.value());
                auto warehouseCondition = CreateRealCondition(Column::warehouse_id, warehouseId);

                auto grouped = CreateGroupCondition(GroupConditionType::AND);
                grouped->conditions.emplace_back(std::move(productCondition));
                grouped->conditions.emplace_back(std::move(warehouseCondition));

                options->condition = std::move(grouped);

                auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

                result_t selectResult;
                auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &selectResult);
                entry->Execute();

                if (isReducing && selectResult.empty())
                {
                    throw pqxx::integrity_constraint_violation("WAS: no such Warehouse_Item");
                }

                if (predicate(products[i], selectResult))
                {
                    toDeleteIds.push_back(products[i].id.value());
                    toCreateCounts.push_back(products[i].count.value());
                }
                else
                {
                    toModifyIdsTarget.emplace_back(products[i].id.value(), calcTarget(products[i], selectResult));
                }
            }

            // handle all critical decisions (delete, insert products)
            if (!toDeleteIds.empty())
            {
                if (isReducing)
                {
                    TRACE_DBG << TRACE_HEADER << "Deleting " << toDeleteIds.size() << " items";

                    auto options = std::make_unique<DeleteOptions>();
                    DeleteValues values;

                    options->table = Table::Warehouse_Item;

                    auto productCondition = CreateInCondition(Column::product_id, std::move(toDeleteIds));
                    auto warehouseCondition = CreateRealCondition(Column::warehouse_id, warehouseId, RealConditionType::Equal);

                    auto condition = CreateGroupCondition(GroupConditionType::AND);
                    condition->conditions.emplace_back(std::move(productCondition));
                    condition->conditions.emplace_back(std::move(warehouseCondition));

                    options->condition = std::move(condition);

                    auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

                    result_t deleteResults;
                    auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), false, &deleteResults);
                    entry->Execute();
                }
                else
                {
                    TRACE_DBG << TRACE_HEADER << "Inserting " << toDeleteIds.size() << " items";

                    auto options = std::make_unique<InsertOptions>();
                    InsertValues values;

                    options->table = Table::Warehouse_Item;
                    options->columns = {Column::warehouse_id, Column::product_id, Column::count};

                    CHECK_TRUE(toDeleteIds.size() == toCreateCounts.size());
                    for (size_t i = 0; i < toDeleteIds.size(); ++i)
                    {
                        params_t params;
                        params.Append(warehouseId).Append(toDeleteIds[i]).Append(toCreateCounts[i]);
                        values.values.emplace_back(std::move(params));
                    }

                    auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

                    result_t insertResults;
                    auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), false, &insertResults);
                    entry->Execute();
                }
            }

            // handle all products modifyings
            if (!toModifyIdsTarget.empty())
            {
                TRACE_DBG << TRACE_HEADER << "Modifying " << toModifyIdsTarget.size() << " items";

                for (const auto& modifyPair : toModifyIdsTarget)
                {
                    auto options = std::make_unique<UpdateOptions>();
                    UpdateValues values;

                    options->table = Table::Warehouse_Item;
                    options->columns = {Column::count};

                    auto productCondition = CreateRealCondition(Column::product_id, modifyPair.first, RealConditionType::Equal);
                    auto warehouseCondition = CreateRealCondition(Column::warehouse_id, warehouseId, RealConditionType::Equal);

                    auto condition = CreateGroupCondition(GroupConditionType::AND);
                    condition->conditions.emplace_back(std::move(productCondition));
                    condition->conditions.emplace_back(std::move(warehouseCondition));

                    options->condition = std::move(condition);

                    values.values.Append(modifyPair.second);

                    auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

                    result_t modifyResults;
                    auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), false, &modifyResults);
                    entry->Execute();
                }
            }
        };

        return entriesFactory.CreateVariableTransactionEntry(std::move(function));
    }

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> SelectByWarehouse(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        int64_t warehouseId,
        std::map<int64_t, Product>& products)
    {
        using namespace srv::db;

        auto results = std::make_shared<result_t>();

        const auto function = [tracer, &entriesFactory, warehouseId, results]() -> void
        {
            auto options = std::make_unique<SelectOptions>();
            SelectValues values;

            options->table = Table::Warehouse_Item;
            options->columns = {Column::product_id, Column::count};

            auto condition = CreateRealCondition(Column::warehouse_id, warehouseId);

            options->condition = std::move(condition);

            auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

            auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), false, results.get());
            entry->Execute();
        };

        auto selectEntry = entriesFactory.CreateVariableTransactionEntry(std::move(function));

        auto converter = [results = std::move(results), &products]() -> void
        {
            for (const auto& row : *results)
            {
                auto& product = products[row.at(0).get<int64_t>().value()];  // edit in-place

                product.id = row.at(0).get<int64_t>().value();
                product.count = row.at(1).get<int64_t>().value();
            }
        };

        auto converterEntry = entriesFactory.CreateVariableTransactionEntry(std::move(converter));

        std::list<std::unique_ptr<ITransactionEntry>> entries;
        entries.emplace_back(std::move(selectEntry));
        entries.emplace_back(std::move(converterEntry));
        return entriesFactory.CreateGroupedTransactionEntry(std::move(entries));
    }
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_C9453D73_17F3_4A71_9370_943429690DFD