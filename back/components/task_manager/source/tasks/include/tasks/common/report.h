#ifndef H_32EBB3EC_8C1E_459B_B816_0845E73CB665
#define H_32EBB3EC_8C1E_459B_B816_0845E73CB665

#include <optional>
#include <string>

#include <instrumental/serialized_enum.h>
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

DEFINE_ENUM_WITH_SERIALIZATION(taskmgr::tasks, ReportType, ByPeriod, Current);

namespace taskmgr
{
namespace tasks
{

struct Report
{
    std::optional<int64_t> report_id;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<timestamp_t> period_from;
    std::optional<timestamp_t> period_to;
    std::optional<int64_t> warehouse_id;
    std::optional<userid_t> created_by;
    std::optional<timestamp_t> created_date;
    std::optional<std::string> filepath;
    std::optional<ReportType> report_type;

    static inline std::unique_ptr<srv::db::IQueryTransactionEntry> InsertEntry(std::shared_ptr<srv::ITracer> tracer,
        srv::db::ITransactionEntryFactory& entriesFactory,
        Report& report,
        srv::IDateProvider& dateProvider)
    {
        using namespace srv::db;

        report.created_date = dateProvider.GetTimestamp();

        auto results = std::make_shared<result_t>();

        const auto function = [tracer = tracer, &entriesFactory, &report, results]() -> void
        {
            auto options = std::make_unique<InsertOptions>();
            InsertValues values;

            options->table = Table::Report;
            options->columns =
                {Column::name, Column::report_type, Column::filepath, Column::created_date, Column::created_by, Column::warehouse_id};
            options->returning = {Column::report_id};

            params_t params;
            params.Append(report.name.value())
                .Append(static_cast<int32_t>(report.report_type.value()))
                .Append(report.filepath.value())
                .Append(report.created_date.value())
                .Append(report.created_by.value())
                .Append(report.warehouse_id.value());

            AppendToQueryOptional(options->columns, params, report.description, Column::description);

            values.values.emplace_back(std::move(params));

            auto query = QueryFactory::Create(tracer, std::move(options), std::move(values));

            auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, results.get());
            entry->Execute();
        };

        auto selectEntry = entriesFactory.CreateVariableTransactionEntry(std::move(function));

        auto converter = [results = std::move(results), &report]() -> void
        {
            report.report_id = results->at(0, 0).get<int64_t>().value();
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

#endif  // H_32EBB3EC_8C1E_459B_B816_0845E73CB665