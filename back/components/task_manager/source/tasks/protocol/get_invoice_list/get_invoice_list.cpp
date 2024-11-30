#include <instrumental/time.h>
#include <instrumental/types.h>
#include <instrumental/user.h>

#include <json/json_helpers.h>

#include <authorizer/authorizer.h>
#include <authorizer/user_info.h>
#include <db_connector/product_definitions/columns.h>
#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tasks/common/invoice_item.h>
#include <tasks/common/warehouse_item.h>
#include <tracer/tracer.h>

#include <tasks/common/filter.h>

#include "get_invoice_list.h"

namespace taskmgr
{
namespace tasks
{

GetInvoiceList::GetInvoiceList(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result GetInvoiceList::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    const auto createResult = ActualGetInvoiceList(*accessor);

    if (createResult == ufa::Result::SUCCESS)
    {
        json::array_t jsonInvoices;

        for (const auto& invoice : m_invoices)
        {
            json jsonInvoice;

            util::json::Put(jsonInvoice, INVOICE_ID_KEY, invoice.invoice_id.value());
            util::json::Put(jsonInvoice, WAREHOUSE_TO_KEY, invoice.warehouse_to_id);
            util::json::Put(jsonInvoice, WAREHOUSE_FROM_KEY, invoice.warehouse_from_id);
            util::json::Put(jsonInvoice, NAME_KEY, invoice.name.value());
            util::json::Put(jsonInvoice, PRETTY_NAME_KEY, invoice.pretty_name.value());
            util::json::Put(jsonInvoice, DESCRIPTION_KEY, invoice.description);
            util::json::Put(jsonInvoice, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(invoice.created_date.value()));
            util::json::Put(jsonInvoice, CREATED_BY_KEY, invoice.created_by.value());

            json::array_t jsonItems;
            for (const auto& item : m_invoiceItems[invoice.invoice_id.value()])
            {
                json jsonItem;

                util::json::Put(jsonItem, INVOICE_ID_KEY, item.invoice_id.value());
                util::json::Put(jsonItem, PRODUCT_ID_KEY, item.product_id.value());
                util::json::Put(jsonItem, COUNT_KEY, item.count.value());

                jsonItems.emplace_back(std::move(jsonItem));
            }
            jsonInvoice[ITEMS_KEY] = std::move(jsonItems);

            jsonInvoices.emplace_back(std::move(jsonInvoice));
        }

        jsonResult[RESULT_KEY] = std::move(jsonInvoices);
    }

    result = jsonResult.dump();
    return createResult;
}

void GetInvoiceList::ParseInternal(json&& json)
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

ufa::Result GetInvoiceList::ActualGetInvoiceList(srv::IAccessor& accessor)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();

    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::Invoice;
    options->columns = {Column::invoice_id,
        Column::warehouse_to_id,
        Column::warehouse_from_id,
        Column::name,
        Column::pretty_name,
        Column::description,
        Column::created_date,
        Column::created_by};

    options->orderBy = Column::invoice_id;

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

    result_t invoiceResults;
    auto selectEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &invoiceResults);

    result_t invoiceItemResults;
    const auto function = [&invoiceResults, &entriesFactory, &invoiceItemResults, this]() -> void
    {
        this->m_invoices.reserve(invoiceResults.size());

        for (const auto& row : invoiceResults)
        {
            Invoice invoice;
            int i = 0;

            invoice.invoice_id = row.at(i++).get<int64_t>().value();
            invoice.warehouse_to_id = row.at(i++).get<int64_t>();
            invoice.warehouse_from_id = row.at(i++).get<int64_t>();
            invoice.name = row.at(i++).get<std::string>().value();
            invoice.pretty_name = row.at(i++).get<std::string>().value();
            invoice.description = row.at(i++).get<std::string>();
            invoice.created_date = row.at(i++).get<timestamp_t>().value();
            invoice.created_by = row.at(i++).get<userid_t>().value();

            this->m_invoices.emplace_back(std::move(invoice));
        }

        std::vector<int64_t> invoicesIds;
        invoicesIds.reserve(this->m_invoices.size());

        for (const auto& invoice : this->m_invoices)
        {
            invoicesIds.push_back(invoice.invoice_id.value());
        }

        if (!invoicesIds.empty())
        {
            auto options = std::make_unique<SelectOptions>();
            SelectValues values;

            options->table = Table::Invoice_Item;
            options->columns = {Column::invoice_id, Column::product_id, Column::count};

            options->orderBy = Column::invoice_id;

            auto condition = CreateInCondition(Column::invoice_id, invoicesIds);
            options->condition = std::move(condition);

            auto query = QueryFactory::Create(GetTracer(), std::move(options), std::move(values));

            auto invoiceItemEntry = entriesFactory.CreateQueryTransactionEntry(std::move(query), true, &invoiceItemResults);
            invoiceItemEntry->Execute();
        }
    };

    auto converterEntry = entriesFactory.CreateVariableTransactionEntry(std::move(function));

    selectEntry->SetNext(std::move(converterEntry));
    transaction->SetRootEntry(std::move(selectEntry));

    auto transactionResult = transaction->Execute();

    if (invoiceResults.empty())
    {
        transactionResult = ufa::Result::NOT_FOUND;
    }
    else if (transactionResult == ufa::Result::SUCCESS)
    {
        for (const auto& row : invoiceItemResults)
        {
            InvoiceItem item;
            int i = 0;

            item.invoice_id = row.at(i++).get<int64_t>();
            item.product_id = row.at(i++).get<int64_t>();
            item.count = row.at(i++).get<int64_t>();

            m_invoiceItems[item.invoice_id.value()].emplace_back(std::move(item));
        }
    }

    return transactionResult;
}

}  // namespace tasks
}  // namespace taskmgr
