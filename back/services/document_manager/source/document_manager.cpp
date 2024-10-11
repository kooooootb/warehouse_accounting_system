#include <filesystem>
#include <memory>

#include <document_manager/document_manager.h>
#include <instrumental/common.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <tracer/tracer.h>

#include "document_manager.h"

namespace srv
{
namespace docmgr
{

DocumentManager::DocumentManager(const std::shared_ptr<srv::IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
{
    TRACE_INF << TRACE_HEADER;

    SetRoot(DEFAULT_DOCUMENT_ROOT);
}

ufa::Result DocumentManager::RestoreDocument(std::filesystem::path& relPath, bool checkExistance)
{
    if (relPath.empty())
    {
        relPath = m_rootPath / INDEX_FILENAME;
        return ufa::Result::SUCCESS;
    }

    relPath = relPath.lexically_normal();
    if (relPath.is_absolute() || !relPath.has_filename())
    {
        TRACE_ERR << TRACE_HEADER << "Requested invalid file: " << relPath.c_str() << "\"";
        return ufa::Result::WRONG_FORMAT;
    }

    relPath = m_rootPath / relPath;

    if (checkExistance && !std::filesystem::exists(relPath))
    {
        TRACE_ERR << TRACE_HEADER << "Requested file not exists: " << relPath.c_str() << "\"";
        return ufa::Result::NOT_FOUND;
    }

    TRACE_INF << TRACE_HEADER << "Restored filename: " << relPath.c_str();
    return ufa::Result::SUCCESS;
}

void DocumentManager::SetRoot(std::filesystem::path&& rootPath)
{
    m_rootPath = std::move(rootPath);
}

}  // namespace docmgr
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IDocumentManager, srv::docmgr::DocumentManager);
