#include <filesystem>
#include <memory>
#include <mutex>

#include <document_manager/document_manager.h>
#include <instrumental/common.h>
#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "back/services/settings_provider/include/settings_provider/settings_provider.h"
#include "document_manager.h"

namespace srv
{
namespace docmgr
{

constexpr std::string_view INDEX_FILENAME = "index.html";
constexpr std::string_view DEFAULT_DOCUMENT_ROOT = ".";

DocumentManager::DocumentManager(const std::shared_ptr<srv::IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
    TRACE_INF << TRACE_HEADER;

    auto settingsProvider = locator->GetInterface<srv::ISettingsProvider>();

    DocumentManagerSettings settings;
    settingsProvider->FillSettings(settings);

    SetSettings(settings);
}

ufa::Result DocumentManager::RestoreDocument(std::filesystem::path& relPath, bool checkExistance)
{
    TRACE_INF << TRACE_HEADER << "relPath: " << relPath << "checkExistance: " << checkExistance;

    if (relPath.empty())
    {
        relPath = INDEX_FILENAME;
    }

    relPath = relPath.lexically_normal();
    if (relPath.is_absolute() || !relPath.has_filename())
    {
        TRACE_ERR << TRACE_HEADER << "Requested invalid file: " << relPath.c_str() << "\"";
        return ufa::Result::WRONG_FORMAT;
    }

    {
        std::lock_guard lock(m_settingsMutex);
        relPath = m_rootPath / relPath;
    }

    if (checkExistance && !std::filesystem::exists(relPath))
    {
        TRACE_ERR << TRACE_HEADER << "Requested file not exists: " << relPath.c_str() << "\"";
        return ufa::Result::NOT_FOUND;
    }

    TRACE_INF << TRACE_HEADER << "Restored filename: " << relPath.c_str();
    return ufa::Result::SUCCESS;
}

void DocumentManager::SetSettings(const docmgr::DocumentManagerSettings& settings)
{
    TRACE_INF << TRACE_HEADER << settings;

    std::lock_guard lock(m_settingsMutex);

    if (settings.documentRoot.has_value())
        m_rootPath = settings.documentRoot.value();
}

}  // namespace docmgr
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IDocumentManager, srv::docmgr::DocumentManager);
