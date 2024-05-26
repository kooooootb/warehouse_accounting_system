#include "dependency_manager.h"
#include <dependency_manager/dependency_manager.h>

namespace taskmgr
{
namespace deps
{

DependencyManager::DependencyManager(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor,
    std::shared_ptr<auth::IAuthorizer> authorizer)
    : srv::tracer::TracerProvider(locator->GetInterface<srv::ITracer>())
    , m_accessor(std::move(accessor))
    , m_authorizer(std::move(authorizer))
{
}

std::unique_ptr<IDependencyManager> IDependencyManager::Create(std::shared_ptr<srv::IServiceLocator> locator,
    std::shared_ptr<db::IAccessor> accessor,
    std::shared_ptr<auth::IAuthorizer> authorizer)
{
    return std::make_unique<DependencyManager>(std::move(locator), std::move(accessor), std::move(authorizer));
}

std::shared_ptr<db::IAccessor> DependencyManager::GetAccessor() const
{
    return m_accessor;
}

std::shared_ptr<auth::IAuthorizer> DependencyManager::GetAuthorizer() const
{
    return m_authorizer;
}

}  // namespace deps
}  // namespace taskmgr
