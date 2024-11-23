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

struct Product
{
    std::optional<std::string> name;
    std::optional<std::string> pretty_name;
    std::optional<std::string> description;
    std::optional<int64_t> main_color;
    std::optional<std::string> main_color_name;
    std::optional<int64_t> count;
    std::optional<int64_t> id;
    std::optional<timestamp_t> created_date;
    std::optional<userid_t> created_by;
    std::optional<int64_t> warehouse_id;

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
        options->returning = {Column::product_id, Column::pretty_name};

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

        return entriesFactory.CreateQueryTransactionEntry(std::move(query), false, &results);
    }

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> SelectEntry(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        std::map<int64_t, Product>& products)
    {
        using namespace srv::db;

        auto results = std::make_shared<result_t>();

        const auto function = [tracer = tracer, &entriesFactory, &products, results]() -> void
        {
            auto options = std::make_unique<SelectOptions>();
            SelectValues values;

            options->table = Table::Product;
            options->columns = {Column::product_id,
                Column::name,
                Column::pretty_name,
                Column::description,
                Column::main_color,
                Column::created_date,
                Column::created_by};

            std::vector<int64_t> productIds;
            productIds.reserve(products.size());
            for (const auto& product : products)
            {
                productIds.push_back(product.first);  // mapped by ids
            }

            if (productIds.empty())
            {
                throw exps::NotFound("received empty product ids");
            }

            auto condition = CreateInCondition(Column::product_id, productIds);

            options->condition = std::move(condition);

            auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

            auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), false, results.get());
            entry->Execute();
        };

        auto selectEntry = entriesFactory.CreateVariableTransactionEntry(std::move(function));

        auto converter = [results = std::move(results), &products]() -> void
        {
            CHECK_TRUE(results->size() == products.size());

            for (const auto& row : *results)
            {
                auto& product = products[row.at(0).get<int64_t>().value()];  // edit in-place

                product.id = row.at(0).get<int64_t>().value();
                product.name = row.at(1).get<std::string>().value();
                product.pretty_name = row.at(2).get<std::string>().value();
                product.description = row.at(3).get<std::string>();
                product.main_color = row.at(4).get<int64_t>();
                product.created_date = row.at(5).get<timestamp_t>().value();
                product.created_by = row.at(6).get<userid_t>().value();
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

#endif  // H_B240D71F_6476_4008_83F1_D6E5D11039DA