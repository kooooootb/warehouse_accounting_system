#ifndef H_B240D71F_6476_4008_83F1_D6E5D11039DA
#define H_B240D71F_6476_4008_83F1_D6E5D11039DA

#include <optional>
#include <string>

#include <instrumental/time.h>
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

#include "helpers.h"

namespace taskmgr
{
namespace tasks
{

struct Product
{
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<int64_t> main_color;
    std::optional<std::string> main_color_name;
    std::optional<int64_t> count;
    std::optional<int64_t> id;
    std::optional<timestamp_t> created_date;
    std::optional<userid_t> created_by;

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> InsertsEntry(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        srv::db::result_t& results,
        std::vector<Product>& products,
        srv::IDateProvider& dateProvider)
    {
        using namespace srv::db;

        auto options = std::make_unique<InsertOptions>();
        InsertValues values;

        options->table = Table::Product;
        options->columns = {Column::name, Column::created_by, Column::created_date, Column::description, Column::main_color};
        options->returning = {Column::product_id};

        for (auto& product : products)
        {
            params_t params;

            product.created_date = dateProvider.GetTimestamp();

            params.Append(product.name.value()).Append(product.created_by.value()).Append(product.created_date.value());

            AppendToQueryNullable(params, product.description);
            AppendToQueryNullable(params, product.main_color);

            values.values.emplace_back(std::move(params));
        }

        auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

        return entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &results);
    }
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_B240D71F_6476_4008_83F1_D6E5D11039DA