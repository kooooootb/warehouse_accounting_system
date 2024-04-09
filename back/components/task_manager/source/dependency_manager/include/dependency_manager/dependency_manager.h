#ifndef H_ACFA7D32_F6D8_47CC_8FA0_8474D4AF0365
#define H_ACFA7D32_F6D8_47CC_8FA0_8474D4AF0365

#include <authorizer/authorizer.h>
#include <db_connector/accessor.h>
#include <instrumental/common.h>
#include <instrumental/interface.h>
#include <locator/service_locator.h>
#include <utilities/document_manager.h>

namespace taskmgr
{
namespace deps
{

struct IDependencyManager : public ufa::IBase
{
    virtual std::shared_ptr<db::IAccessor> GetAccessor() const = 0;
    virtual std::shared_ptr<auth::IAuthorizer> GetAuthorizer() const = 0;
    virtual std::shared_ptr<docmgr::IDocumentManager> GetDocumentManager() const = 0;

    static std::unique_ptr<IDependencyManager> Create(std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<db::IAccessor> accessor,
        std::shared_ptr<auth::IAuthorizer> authorizer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager);
};

}  // namespace deps
}  // namespace taskmgr

#endif  // H_ACFA7D32_F6D8_47CC_8FA0_8474D4AF0365