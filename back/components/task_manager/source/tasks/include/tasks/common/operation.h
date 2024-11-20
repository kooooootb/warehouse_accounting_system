#ifndef H_7EFCF38B_C75A_41EA_8E4B_456D38204530
#define H_7EFCF38B_C75A_41EA_8E4B_456D38204530

#include <optional>
#include <string>

#include <instrumental/settings.h>
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
#include <db_connector/transaction_entry/query_transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry_factory.h>

#include "helpers.h"

namespace taskmgr
{
namespace tasks
{

struct Operation
{
    std::optional<int64_t> operation_id;
    std::optional<int64_t> invoice_id;
    std::optional<std::string> invoice_pretty_name;
    std::optional<int64_t> product_id;
    std::optional<int64_t> warehouse_id;
    std::optional<int64_t> count;
    std::optional<timestamp_t> created_date;
    std::optional<userid_t> created_by;

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> SelectEntryByPeriod(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        timestamp_t from,
        timestamp_t to,
        std::function<void(Operation&&)>&& consumer)
    {
        using namespace srv::db;

        auto options = std::make_unique<SelectOptions>();
        SelectValues values;

        options->table = Table::Operation;
        options->columns = {Column::invoice_id,
            Column::invoice_pretty_name,
            Column::product_id,
            Column::warehouse_id,
            Column::count,
            Column::created_date,
            Column::created_by};

        auto fromCondition = CreateRealCondition(Column::created_date, from, RealConditionType::Greater);
        auto toCondition = CreateRealCondition(Column::created_date, to, RealConditionType::Less);

        auto groupCondition = CreateGroupCondition(GroupConditionType::AND);
        groupCondition->conditions.emplace_back(std::move(fromCondition));
        groupCondition->conditions.emplace_back(std::move(toCondition));

        options->condition = std::move(groupCondition);

        auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

        auto results = std::make_shared<result_t>();
        auto selectEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, results.get());

        auto converter = [results = std::move(results), &consumer]() -> void
        {
            for (const auto& row : *results)
            {
                int i = 0;
                Operation operation;

                operation.invoice_id = row.at(i++).get<int64_t>().value();
                operation.invoice_pretty_name = row.at(i++).get<std::string>().value();
                operation.product_id = row.at(i++).get<int64_t>().value();
                operation.warehouse_id = row.at(i++).get<int64_t>();
                operation.count = row.at(i++).get<int64_t>().value();
                operation.created_date = row.at(i++).get<timestamp_t>().value();
                operation.created_by = row.at(i++).get<userid_t>().value();

                consumer(std::move(operation));
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

#endif  // H_7EFCF38B_C75A_41EA_8E4B_456D38204530