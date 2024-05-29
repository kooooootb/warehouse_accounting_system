#include <filesystem>

#include <db_connector/data/requirement.h>
#include <hash/hash.h>
#include <instrumental/check.h>
#include <instrumental/string_converters.h>
#include <tracer/tracer.h>

#include "task.h"

namespace taskmgr
{
namespace tasks
{

GenerateReport::GenerateReport(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
    : BaseTask(std::move(tracer), std::move(user), std::move(callback))
{
}

ufa::Result GenerateReport::ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result)
{
    result = json().dump();
    auto accessor = depManager.GetAccessor();

    std::vector<db::data::Requirement> reqs;

    const auto getResult = accessor->GetRequirementsBySpecification(m_initiativeUser, m_spec, reqs);

    if (getResult == ufa::Result::SUCCESS)
    {
        const auto documentManager = depManager.GetDocumentManager();
        std::filesystem::path url =
            string_converters::ToString(m_spec.id.value()) + "_" + string_converters::ToString(static_cast<uint64_t>(time(nullptr)));

        json jsonResult;
        jsonResult[URL_KEY] = url;
        result = jsonResult.dump();

        CHECK_SUCCESS(documentManager->RestoreDocument(url));

        std::ofstream fileStream;
        fileStream.open(url, std::ios::out | std::ios::trunc);
        CHECK_TRUE(fileStream.good());

        fileStream << "Report on specification"
                   << "\n";
        fileStream << "Id: " << m_spec.id.value() << "\n";
        fileStream << "Name: " << m_spec.name.value() << "\n";
        fileStream << "Analytic id: " << m_spec.analitycId.value() << "\n";
        fileStream << "Status: " << string_converters::ToString(m_spec.status.value()) << "\n";
        fileStream << "\n";

        for (const auto& req : reqs)
        {
            AppendToFile(fileStream, req);
        }
    }

    return getResult;
}

void GenerateReport::ParseInternal(json&& json)
{
    m_spec.id = json.at(ID_KEY).get<uint64_t>();
}

void GenerateReport::AppendToFile(std::ofstream& fs, const db::data::Requirement& req)
{
    fs << "id: " << req.id.value() << "\n";
    fs << "\t"
       << "description: " << req.description.value() << "\n";
    fs << "\t"
       << "type: " << string_converters::ToString(req.type.value()) << "\n";
    fs << "\t"
       << "status: " << string_converters::ToString(req.status.value()) << "\n";
}

}  // namespace tasks
}  // namespace taskmgr
