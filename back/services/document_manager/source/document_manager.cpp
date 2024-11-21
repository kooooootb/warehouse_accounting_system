#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <random>

#include <instrumental/common.h>
#include <instrumental/settings.h>
#include <instrumental/string_converters.h>
#include <instrumental/types.h>

#include <hash/hash.h>

#include <document_manager/document_manager.h>
#include <settings_provider/settings_provider.h>
#include <tracer/tracer.h>

#include "document_manager.h"

namespace srv
{
namespace docmgr
{

constexpr std::string_view INDEX_FILENAME = "index.html";
constexpr std::string_view DEFAULT_DOCUMENT_ROOT = ".";

DocumentManager::DocumentManager(const std::shared_ptr<srv::IServiceLocator>& locator)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_rootPath(DEFAULT_DOCUMENT_ROOT)
    , m_rng(m_randomDevice())
{
    TRACE_DBG << TRACE_HEADER;

    auto settingsProvider = locator->GetInterface<srv::ISettingsProvider>();

    DocumentManagerSettings settings;
    settingsProvider->FillSettings(settings);

    SetSettings(settings);
}

void DocumentManager::SetSettings(const docmgr::DocumentManagerSettings& settings)
{
    TRACE_INF << TRACE_HEADER << settings;

    std::lock_guard lock(m_settingsMutex);

    if (settings.documentRoot.has_value())
        m_rootPath = settings.documentRoot.value();
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

ufa::Result DocumentManager::RelativizeDocument(std::filesystem::path& relPath)
{
    TRACE_INF << TRACE_HEADER << "relPath: " << relPath;

    try
    {
        auto it = relPath.begin();
        for (auto pathElem : m_rootPath)
        {
            if (it == relPath.end() || *it != pathElem)
            {
                return ufa::Result::WRONG_FORMAT;
            }
            ++it;
        }

        std::filesystem::path result;
        for (; it != relPath.end(); ++it)
        {
            result /= *it;
        }
        relPath = result;

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << ex.what();
        return ufa::Result::ERROR;
    }
}

ufa::Result DocumentManager::CreateFile(std::filesystem::path& relPath, std::ofstream& fstream)
{
    TRACE_INF << TRACE_HEADER << "relPath: " << relPath;

    std::string tweakFactor;
    std::filesystem::path resultPath;

    constexpr int32_t Attempts = 100;
    int32_t attempt = 0;

    while (attempt++ < Attempts)
    {
        resultPath = relPath;
        resultPath = resultPath.concat(tweakFactor);
        auto absolutePath = resultPath;

        if (RestoreDocument(absolutePath, true) == ufa::Result::NOT_FOUND)
        {
            if (CreateDirectoriesAndOpenStream(absolutePath, fstream))
            {
                relPath = resultPath;
                return ufa::Result::SUCCESS;
            }
        }

        tweakFactor = GenerateRandomString();
        TRACE_WRN << TRACE_HEADER << "Path can't be used, tweak with: " << tweakFactor;
    }

    return ufa::Result::ERROR;
}

ufa::Result DocumentManager::EscapePath(std::filesystem::path& path)
{
    TRACE_INF << TRACE_HEADER << "Will escape: " << path;

    // clang-format off
    constexpr std::array<char, 65> SafeCharacters = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '_', '\\', '/'};
    // clang-format on

    constexpr char ReplaceWith = '_';

    std::string pathString = path;

    for (size_t i = 0; i < pathString.size(); ++i)
    {
        char ch = pathString[i];

        if (std::find(std::cbegin(SafeCharacters), std::cend(SafeCharacters), ch) == std::cend(SafeCharacters) &&
            ch != std::filesystem::path::preferred_separator)
        {
            pathString[i] = ReplaceWith;
        }
    }

    path = pathString;
    return ufa::Result::SUCCESS;
}

std::string DocumentManager::GenerateRandomString()
{
    const uint32_t number = static_cast<uint32_t>(m_distribution(m_rng));
    constexpr auto Lsb = 0xFF;

    return util::hash::ToBase64({static_cast<uint8_t>(number & Lsb),
        static_cast<uint8_t>(number >> 8 & Lsb),
        static_cast<uint8_t>(number >> 16 & Lsb),
        static_cast<uint8_t>(number >> 24 & Lsb)});
}

bool DocumentManager::CreateDirectoriesAndOpenStream(const std::filesystem::path path, std::ofstream& fstream)
{
    TRACE_INF << TRACE_HEADER << "path: " << path;

    std::filesystem::path currentPath;
    std::filesystem::path fileName = path.filename();

    for (auto it = path.begin(); it != path.end(); ++it)
    {
        if (*it == fileName)
        {
            break;
        }

        currentPath /= *it;

        if (!std::filesystem::exists(currentPath))
        {
            if (!std::filesystem::create_directory(currentPath))
            {
                return false;
            }
        }
    }

    fstream.open(path, std::ios::out | std::ios::trunc);
    if (fstream.good())
    {
        return true;
    }

    return false;
}

}  // namespace docmgr
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IDocumentManager, srv::docmgr::DocumentManager);
