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

#include "get_invoice.h"

namespace taskmgr
{
namespace tasks
{

GetInvoice::GetInvoice(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetInvoice::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetInvoice(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json jsonInvoice;

        util::json::Put(jsonInvoice, INVOICE_ID_KEY, m_invoice.invoice_id.value());
        util::json::Put(jsonInvoice, WAREHOUSE_TO_KEY, m_invoice.warehouse_to_id);
        util::json::Put(jsonInvoice, WAREHOUSE_FROM_KEY, m_invoice.warehouse_from_id);
        util::json::Put(jsonInvoice, NAME_KEY, m_invoice.name.value());
        util::json::Put(jsonInvoice, PRETTY_NAME_KEY, m_invoice.pretty_name.value());
        util::json::Put(jsonInvoice, DESCRIPTION_KEY, m_invoice.description);
        util::json::Put(jsonInvoice, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_invoice.created_date.value()));
        util::json::Put(jsonInvoice, CREATED_BY_KEY, m_invoice.created_by.value());

        json::array_t jsonItems;
        for (const auto& item : m_invoiceItems)
        {
            json jsonItem;

            util::json::Put(jsonItem, INVOICE_ID_KEY, item.invoice_id.value());
            util::json::Put(jsonItem, PRODUCT_ID_KEY, item.product_id.value());
            util::json::Put(jsonItem, COUNT_KEY, item.count.value());

            jsonItems.emplace_back(std::move(jsonItem));
        }

        jsonInvoice[ITEMS_KEY] = std::move(jsonItems);
        jsonResult[RESULT_KEY] = std::move(jsonInvoice);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetInvoice::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    m_invoice.invoice_id = util::json::Get<int64_t>(json, ID_KEY);
}

ufa::Result GetInvoice::ActualGetInvoice(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Invoice_Item;
    options->columns = {Column::product_id, Column::count};

    auto condition = CreateRealCondition(Column::invoice_id, m_invoice.invoice_id.value());

    options->condition = std::move(condition);

    auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

    result_t itemResults;
    auto invoiceItemEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &itemResults);

    std::vector<Invoice> invoices = {m_invoice};
    auto invoiceEntry = Invoice::SelectEntry(GetTracer(), entriesFactory, invoices);

    invoiceItemEntry->SetNext(std::move(invoiceEntry));

    transaction->SetRootEntry(std::move(invoiceItemEntry));

    auto transactionResult = transaction->Execute();

    if (transactionResult == ufa::Result::SUCCESS)
    {
        m_invoiceItems.reserve(itemResults.size());
        for (const auto& row : itemResults)
        {
            int i = 0;
            InvoiceItem item;

            item.invoice_id = m_invoice.invoice_id.value();
            item.product_id = row.at(i++).get<int64_t>();
            item.count = row.at(i++).get<int64_t>();

            m_invoiceItems.emplace_back(std::move(item));
        }

        m_invoice = std::move(invoices.back());
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
