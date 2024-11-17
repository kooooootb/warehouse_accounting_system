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

#include "products_create.h"

namespace taskmgr
{
namespace tasks
{

ProductsCreate::ProductsCreate(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result ProductsCreate::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = CreateInvoiceProducts(*accessor, *dateProvider);

    if (createResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, INVOICE_ID_KEY, m_invoice.invoice_id.value());
        util::json::Put(jsonResult, INVOICE_CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_invoice.created_date.value()));
        util::json::Put(jsonResult, INVOICE_CREATED_BY_KEY, m_invoice.created_by.value());
        util::json::Put(jsonResult, INVOICE_PRETTY_NAME_KEY, m_invoice.pretty_name.value());
        util::json::Put(jsonResult, INVOICE_NAME_KEY, m_invoice.name.value());
        util::json::Put(jsonResult, INVOICE_DESCRIPTION_KEY, m_invoice.description);
        util::json::Put(jsonResult, WAREHOUSE_TO_KEY, m_invoice.warehouse_to_id.value());

        json::array_t products;

        for (const auto& product : m_products)
        {
            json jsonProduct;

            util::json::Put(jsonProduct, NAME_KEY, product.name.value());
            util::json::Put(jsonProduct, DESCRIPTION_KEY, product.description);
            util::json::Put(jsonProduct, MAIN_COLOR_KEY, product.main_color);
            util::json::Put(jsonProduct, MAIN_COLOR_NAME_KEY, product.main_color_name);
            util::json::Put(jsonProduct, COUNT_KEY, product.count.value());
            util::json::Put(jsonProduct, ID_KEY, product.id.value());
            util::json::Put(jsonProduct, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(product.created_date.value()));
            util::json::Put(jsonProduct, CREATED_BY_KEY, product.created_by.value());
            util::json::Put(jsonProduct, PRETTY_NAME_KEY, product.pretty_name.value());

            products.emplace_back(std::move(jsonProduct));
        }

        jsonResult[PRODUCTS_KEY] = std::move(products);
    }

    result = jsonResult.dump();
    return createResult;
}

void ProductsCreate::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_invoice.name = util::json::Get<std::string>(json, INVOICE_NAME_KEY);
    m_invoice.description = util::json::GetOptional<std::string>(json, INVOICE_DESCRIPTION_KEY);
    m_invoice.warehouse_to_id = util::json::Get<int64_t>(json, WAREHOUSE_TO_KEY);
    m_invoice.created_by = m_initiativeUserId;

    CHECK_TRUE(json.at(PRODUCTS_KEY).is_array());

    for (const auto& productJson : json.at(PRODUCTS_KEY))
    {
        Product product;

        product.name = util::json::Get<std::string>(productJson, NAME_KEY);
        product.description = util::json::GetOptional<std::string>(productJson, DESCRIPTION_KEY);
        product.main_color = util::json::GetOptional<int64_t>(productJson, MAIN_COLOR_KEY);
        product.main_color_name = util::json::GetOptional<std::string>(productJson, MAIN_COLOR_NAME_KEY);
        product.count = util::json::Get<int64_t>(productJson, COUNT_KEY);
        product.created_by = m_initiativeUserId;

        m_products.emplace_back(std::move(product));
    }
}

ufa::Result ProductsCreate::CreateInvoiceProducts(srv::IAccessor& accessor, srv::IDateProvider& dateProvider)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    result_t invoiceInsertResults;
    result_t productInsertsResults;

    const auto idsConverter = [&productInsertsResults, this]() -> void
    {
        CHECK_TRUE(productInsertsResults.size() == this->m_products.size());
        for (size_t i = 0; i < productInsertsResults.size(); ++i)
        {
            this->m_products[i].id = productInsertsResults.at(i, 0).get<int64_t>().value();
            this->m_products[i].pretty_name = productInsertsResults.at(i, 1).get<std::string>().value();
        }
    };

    // as we are 100% creating new products we dont care about updating Warehouse_Item rows and just creating new ones
    auto productsEntry = Product::InsertsEntry(GetTracer(), entriesFactory, productInsertsResults, m_products, dateProvider);
    auto idsConverterEntry = entriesFactory.CreateVariableTransactionEntry(std::move(idsConverter));
    auto invoiceEntry =
        Invoice::FullInsertEntry(GetTracer(), entriesFactory, invoiceInsertResults, m_products, m_invoice, dateProvider);
    auto warehouseItemEntry = WarehouseItem::InsertsEntry(GetTracer(), entriesFactory, m_products, m_invoice.warehouse_to_id.value());

    invoiceEntry->SetNext(std::move(warehouseItemEntry));
    idsConverterEntry->SetNext(std::move(invoiceEntry));
    productsEntry->SetNext(std::move(idsConverterEntry));
    transaction->SetRootEntry(std::move(productsEntry));

    const auto transactionResult = transaction->Execute();

    // all ids are received in entries
    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
