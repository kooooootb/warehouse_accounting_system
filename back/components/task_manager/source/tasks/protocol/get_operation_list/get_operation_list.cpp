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

#include "get_operation_list.h"

namespace taskmgr
{
namespace tasks
{

GetOperationList::GetOperationList(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetOperationList::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetOperationList(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json::array_t jsonOperations;

        for (const auto& operation : m_operations)
        {
            json jsonOperation;

            util::json::Put(jsonOperation, OPERATION_ID_KEY, operation.operation_id.value());
            util::json::Put(jsonOperation, INVOICE_ID_KEY, operation.invoice_id.value());
            util::json::Put(jsonOperation, INVOICE_PRETTY_NAME_KEY, operation.invoice_pretty_name.value());
            util::json::Put(jsonOperation, PRODUCT_ID_KEY, operation.product_id.value());
            util::json::Put(jsonOperation, WAREHOUSE_ID_KEY, operation.warehouse_id);
            util::json::Put(jsonOperation, COUNT_KEY, operation.count.value());
            util::json::Put(jsonOperation, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(operation.created_date.value()));
            util::json::Put(jsonOperation, CREATED_BY_KEY, operation.created_by.value());

            jsonOperations.emplace_back(jsonOperation);
        }

        jsonResult[RESULT_KEY] = std::move(jsonOperations);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetOperationList::ParseInternal(json&& json)
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

ufa::Result GetOperationList::ActualGetOperationList(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Operation;
    options->columns = {Column::operation_id,
        Column::invoice_id,
        Column::invoice_pretty_name,
        Column::product_id,
        Column::warehouse_id,
        Column::count,
        Column::created_date,
        Column::created_by};

    options->orderBy = Column::operation_id;

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
        m_operations.reserve(results.size());

        for (const auto& row : results)
        {
            Operation operation;
            int i = 0;

            operation.operation_id = row.at(i++).get<int64_t>().value();
            operation.invoice_id = row.at(i++).get<int64_t>().value();
            operation.invoice_pretty_name = row.at(i++).get<std::string>().value();
            operation.product_id = row.at(i++).get<int64_t>().value();
            operation.warehouse_id = row.at(i++).get<int64_t>();
            operation.count = row.at(i++).get<int64_t>().value();
            operation.created_date = row.at(i++).get<timestamp_t>().value();
            operation.created_by = row.at(i++).get<userid_t>().value();

            m_operations.emplace_back(operation);
        }
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
