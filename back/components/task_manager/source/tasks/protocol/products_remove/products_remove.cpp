#include <exception>

#include <instrumental/types.h>

#include <json/json_helpers.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <tasks/common/product.h>
#include <tasks/common/warehouse_item.h>

#include "products_remove.h"

namespace taskmgr
{
namespace tasks
{

ProductsRemove::ProductsRemove(std::shared_ptr<srv::ITracer> tracer, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(taskInfo))
{
}

ufa::Result ProductsRemove::ExecuteInternal(const srv::IServiceLocator& locator, std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(locator.GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(locator.GetInterface(dateProvider));

    const auto createResult = RemoveInvoiceProducts(*accessor, *dateProvider);

    if (createResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, INVOICE_ID_KEY, m_invoice.invoice_id.value());
        util::json::Put(jsonResult, INVOICE_CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_invoice.created_date.value()));
        util::json::Put(jsonResult, INVOICE_NAME_KEY, m_invoice.name.value());
        util::json::Put(jsonResult, INVOICE_DESCRIPTION_KEY, m_invoice.description);
        util::json::Put(jsonResult, WAREHOUSE_FROM_KEY, m_invoice.warehouse_from_id.value());

        json::array_t products;

        for (const auto& product : m_products)
        {
            json jsonProduct;

            util::json::Put(jsonProduct, ID_KEY, product.id.value());
            util::json::Put(jsonProduct, COUNT_KEY, product.count.value());

            products.emplace_back(std::move(jsonProduct));
        }

        jsonResult[PRODUCTS_KEY] = std::move(products);
    }

    result = jsonResult.dump();
    return createResult;
}

void ProductsRemove::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_invoice.name = util::json::Get<std::string>(json, INVOICE_NAME_KEY);
    m_invoice.description = util::json::GetOptional<std::string>(json, INVOICE_DESCRIPTION_KEY);
    m_invoice.warehouse_from_id = util::json::Get<int64_t>(json, WAREHOUSE_FROM_KEY);
    m_invoice.created_by = m_initiativeUserId;

    CHECK_TRUE(json.at(PRODUCTS_KEY).is_array());

    for (const auto& productJson : json.at(PRODUCTS_KEY))
    {
        Product product;

        product.id = util::json::Get<int64_t>(productJson, ID_KEY);
        product.count = util::json::Get<int64_t>(productJson, COUNT_KEY);

        m_products.emplace_back(std::move(product));
    }
}

ufa::Result ProductsRemove::RemoveInvoiceProducts(srv::IAccessor& accessor, srv::IDateProvider& dateProvider)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    result_t invoiceInsertResults;

    auto invoiceEntry =
        Invoice::FullInsertEntry(GetTracer(), entriesFactory, invoiceInsertResults, m_products, m_invoice, dateProvider);
    // it is possible to remove only part of product so we care about updating rows in Warehouse_Item
    auto warehouseItemFromEntry =
        WarehouseItem::ChangeCount(GetTracer(), entriesFactory, m_products, m_invoice.warehouse_from_id.value(), true);

    invoiceEntry->SetNext(std::move(warehouseItemFromEntry));
    transaction->SetRootEntry(std::move(invoiceEntry));

    const auto transactionResult = transaction->Execute();

    if (transactionResult == ufa::Result::SUCCESS)
    {
        m_invoice.invoice_id = invoiceInsertResults.at(0, 0).get<int64_t>();
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr