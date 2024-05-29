#include <filesystem>
#include <memory>

#include <instrumental/common.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>
#include <utilities/document_manager.h>

#include "document_manager.h"

namespace docmgr
{

DocumentManager::DocumentManager(std::shared_ptr<srv::ITracer> tracer) : srv::tracer::TracerProvider(std::move(tracer))
{
    SetRoot(DEFAULT_DOCUMENT_ROOT);
}

std::unique_ptr<IDocumentManager> IDocumentManager::Create(std::shared_ptr<srv::ITracer> tracer)
{
    return std::make_unique<DocumentManager>(std::move(tracer));
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
