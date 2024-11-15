#ifndef H_FD52639B_13F5_430A_8603_D5D614294D38
#define H_FD52639B_13F5_430A_8603_D5D614294D38

#include <optional>
#include <string>

#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/product_definitions/columns.h>
#include <db_connector/query/insert_query_params.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/query_transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <db_connector/transaction_entry/transaction_entry_factory.h>
#include <tasks/common/helpers.h>

namespace taskmgr
{
namespace tasks
{

struct Invoice
{
    std::optional<int64_t> invoice_id;
    std::optional<int64_t> warehouse_to_id;
    std::optional<int64_t> warehouse_from_id;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<timestamp_t> created_date;
    std::optional<userid_t> created_by;

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> InsertEntry(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        srv::db::result_t& results,
        Invoice& invoice,
        srv::IDateProvider& dateProvider)
    {
        using namespace srv::db;

        CHECK_TRUE(invoice.warehouse_from_id.has_value() || invoice.warehouse_to_id.has_value());

        invoice.created_date = dateProvider.GetTimestamp();

        auto options = std::make_unique<InsertOptions>();
        InsertValues values;

        options->table = Table::Invoice;
        options->columns = {Column::name, Column::created_by, Column::created_date};
        options->returning = {Column::invoice_id};

        params_t params;
        params.Append(invoice.name.value()).Append(invoice.created_by.value()).Append(invoice.created_date.value());

        if (invoice.warehouse_from_id.has_value())
        {
            options->columns.emplace_back(Column::warehouse_from_id);
            params.Append(invoice.warehouse_from_id.value());
        }

        if (invoice.warehouse_to_id.has_value())
        {
            options->columns.emplace_back(Column::warehouse_to_id);
            params.Append(invoice.warehouse_to_id.value());
        }

        AppendToQueryOptional(options->columns, params, invoice.description, Column::description);

        values.values.emplace_back(std::move(params));

        auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

        return entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &results);
    }
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_FD52639B_13F5_430A_8603_D5D614294D38