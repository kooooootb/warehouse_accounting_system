#ifndef H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
#define H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74

#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>
#include <utilities/document_manager.h>

namespace docmgr
{

class DocumentManager : public srv::tracer::TracerProvider, public IDocumentManager
{
public:
    DocumentManager(std::shared_ptr<srv::ITracer> tracer);

    ufa::Result RestoreDocument(std::filesystem::path& relPath, bool checkExistance = false) override;
    void SetRoot(std::filesystem::path&& rootPath) override;

private:
    std::filesystem::path m_rootPath;
};

}  // namespace docmgr

#endif  // H_434BFF62_FB70_4BF4_A0E6_34F096BEBC74
