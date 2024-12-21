#include <exception>

#include <instrumental/string_converters.h>
#include <instrumental/time.h>
#include <instrumental/types.h>

#include <json/json_helpers.h>

#include <date_provider/date_provider.h>
#include <db_connector/accessor.h>
#include <db_connector/query/utilities.h>
#include <db_connector/transaction_entry/transaction_entry.h>
#include <document_manager/document_manager.h>
#include <locator/service_locator.h>
#include <tasks/common/invoice.h>
#include <tasks/common/report.h>
#include <tracer/tracer.h>

#include <tasks/common/product.h>
#include <tasks/common/warehouse_item.h>

#include "reports_current.h"

namespace taskmgr
{
namespace tasks
{

namespace
{

constexpr std::string_view OpenHtml = R"(
<!DOCTYPE html>
<html lang="en">
)";

constexpr std::string_view Head = R"(
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Report - Report 1</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            color: #333;
            margin: 0;
            padding: 20px;
        }

        header {
            background-color: #333;
            color: #fff;
            padding: 20px;
            text-align: center;
        }

        section {
            background-color: #fff;
            margin: 20px 0;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
        }

        h1,
        h2,
        h3 {
            margin: 0 0 10px 0;
        }

        .invoice {
            border-top: 2px solid #333;
            margin-top: 20px;
            padding-top: 20px;
        }

        .product {
            background-color: #f9f9f9;
            margin-bottom: 10px;
            padding: 10px;
            border-radius: 5px;
        }

        .product-details {
            margin-left: 20px;
        }

        .table-container {
            overflow-x: auto;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-bottom: 20px;
        }
        table th, table td {
            border: 1px solid #ddd;
            padding: 8px;
            text-align: left;
        }
        table th {
            background-color: #f4f4f4;
        }
    </style>
</head>
)";

constexpr std::string_view OpenBody = R"(
<body>
)";

void PrintHeader(std::ostream& os, const Report& report, const Warehouse& warehouse, const srv::IDateProvider& dateProvider)
{
    constexpr std::string_view HeaderTemplate = R"(
    <header>
        <h1>Report #{} - {}</h1>
        <p>Description: {}</p>
        <p>Generated for warehouse: {} - {}</p>
        <p>Created on: {}</p>
    </header>
    )";

    os << fmt::format(HeaderTemplate,
        report.report_id.value(),
        report.name.value(),
        report.description.value_or(""),
        report.warehouse_id.value(),
        warehouse.name.value(),
        dateProvider.ToReadableTimeString(report.created_date.value()));
}

void PrintSummary(std::ostream& os, size_t productsTotal)
{
    constexpr std::string_view SummaryTemplate = R"(
    <section>
        <h2>Summary</h2>
        <p>Total Products: {}</p>
    </section>
    )";

    os << fmt::format(SummaryTemplate, productsTotal);
}

constexpr std::string_view OpenProducts = R"(
    <div class="table-container">
        <table>
            <thead>
                <tr>
                    <th>Product ID</th>
                    <th>Name</th>
                    <th>Pretty Name</th>
                    <th>Description</th>
                    <th>Count</th>
                    <th>Created Date</th>
                </tr>
            </thead>
            <tbody>
)";

void PrintProduct(std::ostream& os, const Product& product, const srv::IDateProvider& dateProvider)
{
    constexpr std::string_view ProductTemplate = R"(
                <tr>
                    <td>{}</td>
                    <td>{}</td>
                    <td>{}</td>
                    <td>{}</td>
                    <td>{}</td>
                    <td>{}</td>
                </tr>
    )";

    os << fmt::format(ProductTemplate,
        product.id.value(),
        product.name.value(),
        product.pretty_name.value(),
        product.description.value_or(""),
        product.count.value(),
        dateProvider.ToReadableTimeString(product.created_date.value()));
}

constexpr std::string_view CloseProducts = R"(
            </tbody>
        </table>
    </div>
</section>
)";

constexpr std::string_view CloseBody = R"(
</body>
)";

constexpr std::string_view CloseHtml = R"(
</html>
)";

}  // namespace

ReportsCurrent::ReportsCurrent(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IServiceLocator> locator,
    const TaskInfo& taskInfo)
    : BaseTask(std::move(tracer), std::move(locator), std::move(taskInfo))
{
}

ufa::Result ReportsCurrent::ExecuteInternal(std::string& result)
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
        util::json::Put(jsonResult, WAREHOUSE_ID_KEY, m_report.warehouse_id.value());

        json::array_t jsonProducts;
        for (const auto& productPair : m_products)
        {
            json jsonProduct;
            const auto& product = productPair.second;

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

        jsonResult[PRODUCTS_KEY] = std::move(jsonProducts);
    }

    result = jsonResult.dump();
    return createResult;
}

void ReportsCurrent::ParseInternal(json&& json)
{
    TRACE_INF << TRACE_HEADER << "Parsing " << GetIdentificator();

    std::shared_ptr<srv::IDateProvider> dateProvider;
    CHECK_SUCCESS(m_locator->GetInterface(dateProvider));

    m_report.name = util::json::Get<std::string>(json, NAME_KEY);
    m_report.description = util::json::GetOptional<std::string>(json, DESCRIPTION_KEY);
    m_report.warehouse_id = util::json::Get<int64_t>(json, WAREHOUSE_ID_KEY);
    m_report.created_by = m_initiativeUserId;

    m_warehouse.warehouse_id = m_report.warehouse_id;
    m_report.report_type = ReportType::Current;
}

ufa::Result ReportsCurrent::CollectData(srv::IAccessor& accessor,
    srv::IDateProvider& dateProvider,
    srv::IDocumentManager& documentManager)
{
    using namespace srv::db;

    std::unique_ptr<ITransaction> transaction;
    CHECK_SUCCESS(accessor.CreateTransaction(transaction));

    auto& entriesFactory = transaction->GetEntriesFactory();
    std::ofstream fstream;

    auto createReportFile = [this, &dateProvider, &documentManager, &fstream]() -> void
    {
        this->CreateReportFile(dateProvider, documentManager, fstream);
    };

    auto warehouseEntry = Warehouse::SelectEntry(GetTracer(), entriesFactory, m_warehouse);
    auto warehouseItemEntry =
        WarehouseItem::SelectByWarehouse(GetTracer(), entriesFactory, m_warehouse.warehouse_id.value(), m_products);
    auto productsEntry = Product::SelectEntry(GetTracer(), entriesFactory, m_products);
    auto createReportFileEntry = entriesFactory.CreateVariableTransactionEntry(std::move(createReportFile));
    auto reportEntry = Report::InsertEntry(GetTracer(), entriesFactory, m_report, dateProvider);

    createReportFileEntry->SetNext(std::move(reportEntry));
    productsEntry->SetNext(std::move(createReportFileEntry));
    warehouseItemEntry->SetNext(std::move(productsEntry));
    warehouseEntry->SetNext(std::move(warehouseItemEntry));
    transaction->SetRootEntry(std::move(warehouseEntry));

    const auto transactionResult = transaction->Execute();

    WriteReportFile(dateProvider, documentManager, fstream);

    return transactionResult;
}

ufa::Result ReportsCurrent::CreateReportFile(srv::IDateProvider& dateProvider,
    srv::IDocumentManager& documentManager,
    std::ofstream& fstream)
{
    constexpr std::string_view ReportsFolder = "reports";

    std::filesystem::path reportPath = ReportsFolder;
    reportPath.append(string_converters::ToString(m_report.report_type.value()));
    reportPath.append(dateProvider.ToIsoTimeString(m_report.created_date.value()) + ".html");

    CHECK_SUCCESS(documentManager.EscapePath(reportPath));

    CHECK_SUCCESS(documentManager.CreateFile(reportPath, fstream));

    TRACE_INF << TRACE_HEADER << "Writing report in file: " << reportPath;
    m_report.filepath = reportPath;

    return ufa::Result::SUCCESS;
}

ufa::Result ReportsCurrent::WriteReportFile(srv::IDateProvider& dateProvider,
    srv::IDocumentManager& documentManager,
    std::ofstream& fstream)
{
    fstream << OpenHtml;
    fstream << Head;
    fstream << OpenBody;

    PrintHeader(fstream, m_report, m_warehouse, dateProvider);

    PrintSummary(fstream, m_products.size());

    fstream << OpenProducts;

    for (const auto& productPair : m_products)
    {
        const auto& product = productPair.second;

        PrintProduct(fstream, product, dateProvider);
    }

    fstream << CloseProducts;

    fstream << CloseBody;
    fstream << CloseHtml;

    return ufa::Result::SUCCESS;
}

}  // namespace tasks
}  // namespace taskmgr
