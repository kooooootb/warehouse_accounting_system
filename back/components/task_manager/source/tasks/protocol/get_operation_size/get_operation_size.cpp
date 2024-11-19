#include <db_connector/query/query_factory.h>
#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tasks/common/warehouse_item.h>
#include <tracer/tracer.h>

#include "get_operation_size.h"

namespace taskmgr
{
namespace tasks
{

GetOperationSize::GetOperationSize(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetOperationSize::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    int32_t count;

    const auto createResult = ActualGetOperationSize(*accessor, count);

    if (createResult == ufa::Result::SUCCESS)
    {
        jsonResult[COUNT_KEY] = std::move(count);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetOperationSize::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();
}

ufa::Result GetOperationSize::ActualGetOperationSize(srv::IAccessor& accessor, int32_t& count)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto query = QueryFactory::CreateRaw(GetTracer(), "SELECT COUNT(*) FROM public.\"Operation\";", {});

    result_t results;
    auto entry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &results);

    transaction->SetRootEntry(std::move(entry));

    auto transactionResult = transaction->Execute();

    if (transactionResult == ufa::Result::SUCCESS)
    {
        count = results.at(0, 0).get<int32_t>().value();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
