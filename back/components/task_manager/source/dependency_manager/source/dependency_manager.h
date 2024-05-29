#ifndef H_C553F798_4DAC_4F39_A5DB_F24298B8E44F
#define H_C553F798_4DAC_4F39_A5DB_F24298B8E44F

#include <dependency_manager/dependency_manager.h>
#include <tracer/tracer_provider.h>
#include <utilities/document_manager.h>

namespace taskmgr
{
namespace deps
{

class DependencyManager : public srv::tracer::TracerProvider, public IDependencyManager
{
public:
    DependencyManager(std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<db::IAccessor> accessor,
        std::shared_ptr<auth::IAuthorizer> authorizer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager);

    std::shared_ptr<db::IAccessor> GetAccessor() const override;
    std::shared_ptr<auth::IAuthorizer> GetAuthorizer() const override;
    std::shared_ptr<docmgr::IDocumentManager> GetDocumentManager() const override;

private:
    std::shared_ptr<db::IAccessor> m_accessor;
    std::shared_ptr<auth::IAuthorizer> m_authorizer;
    std::shared_ptr<docmgr::IDocumentManager> m_documentManager;
};

}  // namespace deps
}  // namespace taskmgr

#endif  // H_C553F798_4DAC_4F39_A5DB_F24298B8E44F