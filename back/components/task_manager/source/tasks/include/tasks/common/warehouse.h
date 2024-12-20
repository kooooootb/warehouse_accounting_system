#ifndef H_78975C3A_C382_4DEA_9012_1FDC33521390
#define H_78975C3A_C382_4DEA_9012_1FDC33521390

#include <optional>
#include <string>

#include <instrumental/time.h>
#include <instrumental/user.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/product_definitions/columns.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/insert_query_params.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>
#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/exceptions/not_found_exception.h>
#include <db_connector/transaction_entry/query_transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry_factory.h>

#include "helpers.h"

namespace taskmgr
{
namespace tasks
{

struct Warehouse
{
    std::optional<std::string> name;
    std::optional<std::string> pretty_name;
    std::optional<std::string> description;
    std::optional<std::string> location;
    std::optional<int64_t> warehouse_id;
    std::optional<int64_t> created_date;
    std::optional<int64_t> created_by;

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> InsertEntry(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        srv::db::result_t& results,
        Warehouse& warehouse,
        srv::IDateProvider& dateProvider)
    {
        using namespace srv::db;

        CHECK_TRUE(warehouse.name.has_value());
        CHECK_TRUE(warehouse.created_by.has_value());

        warehouse.created_date = dateProvider.GetTimestamp();

        auto options = std::make_unique<InsertOptions>();
        InsertValues values;

        options->table = Table::Warehouse;
        options->columns = {Column::name, Column::created_by, Column::created_date};
        options->returning = {Column::warehouse_id, Column::pretty_name};

        params_t params;

        params.Append(warehouse.name.value()).Append(warehouse.created_by.value()).Append(warehouse.created_date.value());

        AppendToQueryOptional(options->columns, params, warehouse.description, Column::description);
        AppendToQueryOptional(options->columns, params, warehouse.location, Column::location);

        values.values.emplace_back(std::move(params));

        auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

        return entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &results);
    }

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> SelectEntry(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        Warehouse& warehouse)
    {
        using namespace srv::db;

        CHECK_TRUE(warehouse.warehouse_id.has_value());

        auto options = std::make_unique<SelectOptions>();
        SelectValues values;

        options->table = Table::Warehouse;
        options->columns =
            {Column::name, Column::pretty_name, Column::description, Column::created_date, Column::created_by, Column::location};

        auto condition = CreateRealCondition(Column::warehouse_id, warehouse.warehouse_id.value());

        options->condition = std::move(condition);

        auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

        auto results = std::make_shared<result_t>();
        auto selectEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, results.get());

        auto converter = [results = std::move(results), &warehouse]() -> void
        {
            if (results->empty())
            {
                throw exps::NotFound("no warehouse with given id");
            }

            warehouse.name = results->at(0, 0).get<std::string>().value();
            warehouse.pretty_name = results->at(0, 1).get<std::string>().value();
            warehouse.description = results->at(0, 2).get<std::string>();
            warehouse.created_date = results->at(0, 3).get<timestamp_t>().value();
            warehouse.created_by = results->at(0, 4).get<userid_t>().value();
            warehouse.location = results->at(0, 2).get<std::string>();
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

#endif  // H_78975C3A_C382_4DEA_9012_1FDC33521390
