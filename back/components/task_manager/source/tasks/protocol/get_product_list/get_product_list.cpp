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

#include "get_product_list.h"

namespace taskmgr
{
namespace tasks
{

GetProductList::GetProductList(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetProductList::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetProductList(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json::array_t jsonProducts;

        for (const auto& product : m_products)
        {
            json jsonProduct;

            util::json::Put(jsonProduct, PRODUCT_ID_KEY, product.id.value());
            util::json::Put(jsonProduct, NAME_KEY, product.name.value());
            util::json::Put(jsonProduct, PRETTY_NAME_KEY, product.pretty_name.value());
            util::json::Put(jsonProduct, DESCRIPTION_KEY, product.description);
            util::json::Put(jsonProduct, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(product.created_date.value()));
            util::json::Put(jsonProduct, CREATED_BY_KEY, product.created_by.value());
            util::json::Put(jsonProduct, MAIN_COLOR_KEY, product.main_color);

            jsonProducts.emplace_back(jsonProduct);
        }

        jsonResult[RESULT_KEY] = std::move(jsonProducts);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetProductList::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_limit = util::json::Get<int64_t>(json, LIMIT_KEY);
    m_offset = util::json::Get<int64_t>(json, OFFSET_KEY);

    const auto filtersIt = json.find(FILTERS_KEY);
    if (filtersIt != json.end())
    {
        std::shared_ptr<srv::IDateProvider> dateProvider;
        CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

        m_filter = ParseFilters(GetTracer(), *dateProvider, *filtersIt);
    }
}

ufa::Result GetProductList::ActualGetProductList(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Product;
    options->columns = {Column::product_id,
        Column::name,
        Column::pretty_name,
        Column::description,
        Column::created_date,
        Column::created_by,
        Column::main_color};

    options->orderBy = Column::product_id;
    options->limit = m_limit;
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
        m_products.reserve(results.size());

        for (const auto& row : results)
        {
            Product product;
            int i = 0;

            product.id = row.at(i++).get<int64_t>();
            product.name = row.at(i++).get<std::string>();
            product.pretty_name = row.at(i++).get<std::string>();
            product.description = row.at(i++).get<std::string>();
            product.created_date = row.at(i++).get<timestamp_t>();
            product.created_by = row.at(i++).get<userid_t>();
            product.main_color = row.at(i++).get<int64_t>();

            m_products.emplace_back(product);
        }
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
