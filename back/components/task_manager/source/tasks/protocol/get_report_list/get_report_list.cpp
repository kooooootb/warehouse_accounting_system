#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tasks/common/product.h>
#include <tracer/tracer.h>

#include <tasks/common/filter.h>

#include "get_report_list.h"

namespace taskmgr
{
namespace tasks
{

GetReportList::GetReportList(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetReportList::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetReportList(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json::array_t jsonReports;

        for (const auto& report : m_reports)
        {
            json jsonReport;

            util::json::Put(jsonReport, REPORT_ID_KEY, report.report_id.value());
            util::json::Put(jsonReport, NAME_KEY, report.name.value());
            util::json::Put(jsonReport, DESCRIPTION_KEY, report.description);
            util::json::Put(jsonReport, REPORT_TYPE_KEY, string_converters::ToString(report.report_type.value()));
            util::json::Put(jsonReport, FILEPATH_KEY, report.filepath.value());
            util::json::Put(jsonReport, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(report.created_date.value()));
            util::json::Put(jsonReport, CREATED_BY_KEY, report.created_by);
            util::json::Put(jsonReport, WAREHOUSE_ID_KEY, report.warehouse_id.value());

            jsonReports.emplace_back(jsonReport);
        }

        jsonResult[RESULT_KEY] = std::move(jsonReports);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetReportList::ParseInternal(json&& json)
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

ufa::Result GetReportList::ActualGetReportList(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Report;
    options->columns = {Column::report_id,
        Column::name,
        Column::description,
        Column::report_type,
        Column::filepath,
        Column::created_date,
        Column::created_by,
        Column::warehouse_id};

    options->orderBy = Column::report_id;

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

    result_t results;
    auto selectEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &results);

    transaction->SetRootEntry(std::move(selectEntry));

    auto transactionResult = transaction->Execute();

    if (results.empty())
    {
        transactionResult = ufa::Result::NOT_FOUND;
    }
    else if (transactionResult == ufa::Result::SUCCESS)
    {
        m_reports.reserve(results.size());

        for (const auto& row : results)
        {
            Report report;
            int i = 0;

            report.report_id = row.at(i++).get<int64_t>();
            report.name = row.at(i++).get<std::string>();
            report.description = row.at(i++).get<std::string>();
            report.report_type = static_cast<ReportType>(row.at(i++).get<int64_t>().value());
            report.filepath = row.at(i++).get<std::string>();
            report.created_date = row.at(i++).get<timestamp_t>();
            report.created_by = row.at(i++).get<userid_t>();
            report.warehouse_id = row.at(i++).get<int64_t>().value();

            m_reports.emplace_back(std::move(report));
        }
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
