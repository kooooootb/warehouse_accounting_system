#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include "get_product.h"

namespace taskmgr
{
namespace tasks
{

GetProduct::GetProduct(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetProduct::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetProduct(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, PRODUCT_ID_KEY, m_product.id.value());
        util::json::Put(jsonResult, NAME_KEY, m_product.name.value());
        util::json::Put(jsonResult, PRETTY_NAME_KEY, m_product.pretty_name.value());
        util::json::Put(jsonResult, DESCRIPTION_KEY, m_product.description);
        util::json::Put(jsonResult, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_product.created_date.value()));
        util::json::Put(jsonResult, CREATED_BY_KEY, m_product.created_by.value());
        util::json::Put(jsonResult, MAIN_COLOR_KEY, m_product.main_color);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetProduct::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_product.id = util::json::Get<int64_t>(json, ID_KEY);
}

ufa::Result GetProduct::ActualGetProduct(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Product;
    options->columns =
        {Column::name, Column::pretty_name, Column::description, Column::created_date, Column::created_by, Column::main_color};

    auto condition = CreateRealCondition(Column::product_id, m_product.id.value());

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

        m_product.name = results.at(0, i++).get<std::string>();
        m_product.pretty_name = results.at(0, i++).get<std::string>();
        m_product.description = results.at(0, i++).get<std::string>();
        m_product.created_date = results.at(0, i++).get<timestamp_t>();
        m_product.created_by = results.at(0, i++).get<userid_t>();
        m_product.main_color = results.at(0, i++).get<int64_t>();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
