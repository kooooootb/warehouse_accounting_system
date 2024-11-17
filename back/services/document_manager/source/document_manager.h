#ifndef H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
#define H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74

#include <mutex>
#include <random>

#include <document_manager/document_manager.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace srv
{
namespace docmgr
{

class DocumentManager : public srv::tracer::TracerProvider, public srv::IDocumentManager
{
public:
    DocumentManager(const std::shared_ptr<srv::IServiceLocator>& locator);

    ufa::Result RestoreDocument(std::filesystem::path& relPath, bool checkExistance) override;
    ufa::Result RelativizeDocument(std::filesystem::path& relPath) override;
    ufa::Result CreateFile(std::filesystem::path& relPath, std::ofstream& fstream) override;
    ufa::Result EscapePath(std::filesystem::path& path) override;

    void SetSettings(const docmgr::DocumentManagerSettings& settings) override;

private:
    std::string GenerateRandomString();
    bool CreateDirectoriesAndOpenStream(const std::filesystem::path path, std::ofstream& fstream);

private:
    std::mutex m_settingsMutex;
    std::filesystem::path m_rootPath;

    std::random_device m_randomDevice;
    std::mt19937 m_rng;
    std::uniform_int_distribution<std::mt19937::result_type> m_distribution;
};

}  // namespace docmgr
}  // namespace srv

#endif  // H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
