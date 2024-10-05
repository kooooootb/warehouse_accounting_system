#ifndef H_CD8C6A53_ABF8_4D22_85C7_82DB21735351
#define H_CD8C6A53_ABF8_4D22_85C7_82DB21735351

#include <locator/lazy_service_provider.h>

#include "tracer.h"

namespace srv
{
namespace tracer
{

/**
 * @brief technically is just wrapper around LazyServiceProvider
 */
class TracerLazyProvider : public tools::LazyServiceProvider<ITracer>
{
public:
    TracerLazyProvider(std::weak_ptr<IServiceLocator> locator) : tools::LazyServiceProvider<ITracer>(locator) {}

protected:
    std::shared_ptr<ITracer> GetTracer() const
    {
        return GetLazy();
    }
};

}  // namespace tracer
}  // namespace srv

#endif  // H_CD8C6A53_ABF8_4D22_85C7_82DB21735351
