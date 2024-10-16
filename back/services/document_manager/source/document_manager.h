#ifndef H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
#define H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74

#include <mutex>

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

    void SetSettings(const docmgr::DocumentManagerSettings& settings) override;

private:
    std::mutex m_settingsMutex;
    std::filesystem::path m_rootPath;
};

}  // namespace docmgr
}  // namespace srv

#endif  // H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
