#include <exception>

#include <instrumental/string_converters.h>
#include <instrumental/time.h>
#include <instrumental/types.h>

#include <json/json_helpers.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tasks/common/report.h>
#include <tracer/tracer.h>

#include <tasks/common/product.h>
#include <tasks/common/warehouse_item.h>

#include "back/services/document_manager/include/document_manager/document_manager.h"
#include "reports_by_period.h"

namespace taskmgr
{
namespace tasks
{

ReportsByPeriod::ReportsByPeriod(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result ReportsByPeriod::ExecuteInternal(std::string& result)
{
    TRACE_INF << TRACE_HEADER << "Executing " << GetIdentificator();

    json jsonResult;

    std::shared_ptr<srv::IAccessor> accessor;
    CHECK_SUCCESS(m_locator->GetInterface(accessor));

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    std::shared_ptr<srv::IDocumentManager> documentManager;
    CHECK_SUCCESS(m_locator->GetInterface(documentManager));

    const auto createResult = CollectData(*accessor, *dateProvider, *documentManager);

    if (createResult == ufa::Result::SUCCESS)
    {
        util::json::Put(jsonResult, REPORT_ID_KEY, m_report.report_id.value());
        util::json::Put(jsonResult, FILEPATH_KEY, m_report.filepath.value());
        util::json::Put(jsonResult, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(m_report.created_date.value()));
        util::json::Put(jsonResult, CREATED_BY_KEY, m_report.created_by.value());
        util::json::Put(jsonResult, NAME_KEY, m_report.name.value());
        util::json::Put(jsonResult, DESCRIPTION_KEY, m_report.description);
        util::json::Put(jsonResult, PERIOD_FROM_KEY, dateProvider->ToIsoTimeString(m_report.period_from.value()));
        util::json::Put(jsonResult, PERIOD_TO_KEY, dateProvider->ToIsoTimeString(m_report.period_to.value()));
        util::json::Put(jsonResult, WAREHOUSE_ID_KEY, m_report.warehouse_id.value());

        json::array_t jsonInvoices;
        for (const auto& invoice : m_invoices)
        {
            json jsonInvoice;

            util::json::Put(jsonInvoice, INVOICE_ID_KEY, invoice.invoice_id.value());
            util::json::Put(jsonInvoice, INVOICE_PRETTY_NAME_KEY, invoice.pretty_name.value());
            util::json::Put(jsonInvoice, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(invoice.created_date.value()));
            util::json::Put(jsonInvoice, CREATED_BY_KEY, invoice.created_by.value());
            util::json::Put(jsonInvoice, WAREHOUSE_TO_ID_KEY, invoice.warehouse_to_id);
            util::json::Put(jsonInvoice, WAREHOUSE_FROM_ID_KEY, invoice.warehouse_from_id);

            json::array_t jsonProducts;
            for (const auto& operation : m_operations[invoice.invoice_id.value()])
            {
                json jsonProduct;
                const auto& product = m_products[operation.product_id.value()];

                util::json::Put(jsonProduct, NAME_KEY, product.name.value());
                util::json::Put(jsonProduct, DESCRIPTION_KEY, product.description);
                util::json::Put(jsonProduct, MAIN_COLOR_KEY, product.main_color);
                util::json::Put(jsonProduct, ID_KEY, product.id.value());
                util::json::Put(jsonProduct, COUNT_KEY, product.count.value());
                util::json::Put(jsonProduct, CREATED_DATE_KEY, dateProvider->ToIsoTimeString(product.created_date.value()));
                util::json::Put(jsonProduct, CREATED_BY_KEY, product.created_by.value());
                util::json::Put(jsonProduct, PRETTY_NAME_KEY, product.pretty_name.value());

                jsonProducts.emplace_back(std::move(jsonProduct));
            }
            jsonInvoice[PRODUCTS_KEY] = std::move(jsonProducts);

            jsonInvoices.emplace_back(std::move(jsonInvoice));
        }

        jsonResult[INVOICES_KEY] = std::move(jsonInvoices);
    }

    result = jsonResult.dump();
    return createResult;
}

void ReportsByPeriod::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    m_report.name = util::json::Get<std::string>(json, NAME_KEY);
    m_report.description = util::json::GetOptional<std::string>(json, DESCRIPTION_KEY);
    m_report.warehouse_id = util::json::Get<int64_t>(json, WAREHOUSE_ID_KEY);
    m_report.created_by = m_initiativeUserId;

    timestamp_t periodFrom, periodTo;
    CHECK_SUCCESS(dateProvider->FromIsoTimeString(util::json::Get<std::string>(json, PERIOD_FROM_KEY), periodFrom));
    CHECK_SUCCESS(dateProvider->FromIsoTimeString(util::json::Get<std::string>(json, PERIOD_TO_KEY), periodTo));
    m_report.period_from = periodFrom;
    m_report.period_to = periodTo;

    m_warehouse.warehouse_id = m_report.warehouse_id;
    m_report.report_type = ReportType::ByPeriod;
}

ufa::Result ReportsByPeriod::CollectData(srv::IAccessor& accessor,
    srv::IDateProvider& dateProvider,
    srv::IDocumentManager& documentManager)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();
    std::ofstream fstream;

    auto operationsConsumer = [this](Operation&& operation) -> void
    {
        this->m_invoices.emplace_back(Invoice{.invoice_id = operation.invoice_id});
        this->m_products[operation.product_id.value()] = Product{.count = operation.count, .id = operation.product_id};
        this->m_operations[operation.invoice_id.value()].emplace_back(std::move(operation));
    };

    auto createReportFile = [this, &dateProvider, &documentManager, &fstream]() -> void
    {
        this->CreateReportFile(dateProvider, documentManager, fstream);
    };

    auto warehouseEntry = Warehouse::SelectEntry(GetTracer(), entriesFactory, m_warehouse);
    auto operationEntry = Operation::SelectEntryByPeriod(GetTracer(),
        entriesFactory,
        m_report.period_from.value(),
        m_report.period_to.value(),
        std::move(operationsConsumer));
    auto invoicesEntry = Invoice::SelectEntry(GetTracer(), entriesFactory, m_invoices);
    auto productsEntry = Product::SelectEntry(GetTracer(), entriesFactory, m_products);
    auto createReportFileEntry = entriesFactory.CreateVariableTransactionEntry(std::move(createReportFile));
    auto reportEntry = Report::InsertEntry(GetTracer(), entriesFactory, m_report, dateProvider);

    createReportFileEntry->SetNext(std::move(reportEntry));
    productsEntry->SetNext(std::move(createReportFileEntry));
    invoicesEntry->SetNext(std::move(productsEntry));
    operationEntry->SetNext(std::move(invoicesEntry));
    warehouseEntry->SetNext(std::move(operationEntry));
    transaction->SetRootEntry(std::move(warehouseEntry));

    const auto transactionResult = transaction->Execute();

    WriteReportFile(dateProvider, documentManager, fstream);

    return transactionResult;
}

ufa::Result ReportsByPeriod::CreateReportFile(srv::IDateProvider& dateProvider,
    srv::IDocumentManager& documentManager,
    std::ofstream& fstream)
{
    constexpr std::string_view ReportsFolder = "reports";

    std::filesystem::path reportPath = ReportsFolder;
    reportPath.append(string_converters::ToString(m_report.report_type.value()));
    reportPath.append(dateProvider.ToIsoTimeString(m_report.created_date.value()));

    CHECK_SUCCESS(documentManager.EscapePath(reportPath));

    CHECK_SUCCESS(documentManager.CreateFile(reportPath, fstream));

    TRACE_INF << TRACE_HEADER << "Writing report in file: " << reportPath;
    m_report.filepath = reportPath;

    return ufa::Result::SUCCESS;
}

ufa::Result ReportsByPeriod::WriteReportFile(srv::IDateProvider& dateProvider,
    srv::IDocumentManager& documentManager,
    std::ofstream& fstream)
{
    fstream << "REPORT id: " << m_report.report_id.value() << ", name:\'" << m_report.name.value() << "\'\n\n";
    fstream << "Generated for period: from " << dateProvider.ToIsoTimeString(m_report.period_from.value()) << " to "
            << dateProvider.ToIsoTimeString(m_report.period_to.value()) << '\n';
    fstream << "Warehouse: id: " << m_warehouse.warehouse_id.value() << ", name: " << m_warehouse.name.value() << "\n\n";
    fstream << "Total: " << m_invoices.size() << " invoices\n\n";

    for (const auto& invoice : m_invoices)
    {
        fstream << "Invoice #" << invoice.invoice_id.value() << " \'" << invoice.name.value() << "\'\n";
        fstream << "\tCreated in " << dateProvider.ToIsoTimeString(invoice.created_date.value()) << '\n';

        if (invoice.warehouse_from_id.has_value())
        {
            fstream << "\tProducts shipped from warehouse " << invoice.warehouse_from_id.value() << '\n';
        }

        if (invoice.warehouse_to_id.has_value())
        {
            fstream << "\tProducts shipped to warehouse " << invoice.warehouse_to_id.value() << '\n';
        }

        fstream << "\tProducts shipped:" << '\n';

        for (const auto& operation : m_operations[invoice.invoice_id.value()])
        {
            const auto& product = m_products[operation.product_id.value()];

            fstream << "\tId : " << product.id.value() << '\n';
            fstream << "\t\tName : " << product.name.value() << '\n';
            fstream << "\t\tPretty_name : " << product.pretty_name.value() << '\n';
            if (product.description.has_value())
                fstream << "\t\tDescription : " << product.description.value() << '\n';
            if (product.main_color.has_value())
                fstream << "\t\tMain_color : " << product.main_color.value() << '\n';
            fstream << "\t\tCount : " << product.count.value() << '\n';
            fstream << "\t\tCreated_date : " << dateProvider.ToIsoTimeString(product.created_date.value()) << '\n';
        }
    }

    return ufa::Result::SUCCESS;
}

}  // namespace tasks
}  // namespace taskmgr
