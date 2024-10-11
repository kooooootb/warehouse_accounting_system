#ifndef H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
#define H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74

#include <document_manager/document_manager.h>
#include <locator/service_locator.h>
#include <tracer/tracer_provider.h>

namespace srv
{
namespace docmgr
{

constexpr std::string_view INDEX_FILENAME = "index.html";
constexpr std::string_view DEFAULT_DOCUMENT_ROOT = ".";

class DocumentManager : public srv::tracer::TracerProvider, public srv::IDocumentManager
{
public:
    DocumentManager(const std::shared_ptr<srv::IServiceLocator>& locator);

    ufa::Result RestoreDocument(std::filesystem::path& relPath, bool checkExistance = false) override;
    void SetRoot(std::filesystem::path&& rootPath) override;

private:
    std::filesystem::path m_rootPath;
};

}  // namespace docmgr
}  // namespace srv

#endif  // H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
