#include <instrumental/string_converters.h>
#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <date_provider/date_provider.h>
#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>
#include <locator/service_locator.h>
#include <tasks/common/report.h>
#include <tracer/tracer.h>

#include "get_report.h"

namespace taskmgr
{
namespace tasks
{

GetReport::GetReport(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetReport::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetReport(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json jsonReport;

        util::json::Put(jsonReport, REPORT_ID_KEY, m_report.report_id.value());
        util::json::Put(jsonReport, NAME_KEY, m_report.name.value());
        util::json::Put(jsonReport, DESCRIPTION_KEY, m_report.description);
        util::json::Put(jsonReport, REPORT_TYPE_KEY, string_converters::ToString(m_report.report_type.value()));
        util::json::Put(jsonReport, FILEPATH_KEY, m_report.filepath.value());
        util::json::Put(jsonReport, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_report.created_date.value()));
        util::json::Put(jsonReport, CREATED_BY_KEY, m_report.created_by);
        util::json::Put(jsonReport, WAREHOUSE_ID_KEY, m_report.warehouse_id.value());

        jsonResult[RESULT_KEY] = std::move(jsonReport);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetReport::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_report.report_id = util::json::Get<int64_t>(json, ID_KEY);
}

ufa::Result GetReport::ActualGetReport(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Report;
    options->columns = {Column::name,
        Column::description,
        Column::report_type,
        Column::filepath,
        Column::created_date,
        Column::created_by,
        Column::warehouse_id};

    auto condition = CreateRealCondition(Column::report_id, m_report.report_id.value());

    options->condition = std::move(condition);

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
        int i = 0;

        m_report.name = results.at(0, i++).get<std::string>();
        m_report.description = results.at(0, i++).get<std::string>();
        m_report.report_type = static_cast<ReportType>(results.at(0, i++).get<int64_t>().value());
        m_report.filepath = results.at(0, i++).get<std::string>();
        m_report.created_date = results.at(0, i++).get<timestamp_t>();
        m_report.created_by = results.at(0, i++).get<userid_t>();
        m_report.warehouse_id = results.at(0, i++).get<int64_t>().value();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
