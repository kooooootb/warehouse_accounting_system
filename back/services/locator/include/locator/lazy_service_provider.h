#ifndef H_E0B79933_027A_4D3F_8C49_E78BA42A62BC
#define H_E0B79933_027A_4D3F_8C49_E78BA42A62BC

#include <memory>

#include <instrumental/common.h>
#include <locator/service_locator.h>

namespace srv
{
namespace tools
{

template <typename T>
struct LazyServiceProvider
{
public:
    LazyServiceProvider(std::weak_ptr<IServiceLocator> locatorWeak);

protected:
    // return shared ptr as this provider doesnt own
    std::shared_ptr<T> GetLazy() const;

    // late initialization especially for IServiceLocator
    void SetLocator(std::weak_ptr<IServiceLocator> locator) const;

private:
    bool TryGetService() const;

private:
    // make it weak because services shouldn't own servicelocator
    mutable std::weak_ptr<IServiceLocator> m_locatorWeak;
    // make it weak because it can break services owning tree
    // mutable for const methods, service dying (and pointer reset) seems to happen rarely
    // todo: do something with mutable
    mutable std::weak_ptr<T> m_serviceWeak;
};

template <typename T>
LazyServiceProvider<T>::LazyServiceProvider(std::weak_ptr<IServiceLocator> locatorWeak) : m_locatorWeak(std::move(locatorWeak))
{
    TryGetService();
}

template <typename T>
bool LazyServiceProvider<T>::TryGetService() const
{
    if (auto locator = m_locatorWeak.lock())
    {
        std::shared_ptr<T> service;
        if (locator->GetInterface(service) == ufa::Result::SUCCESS)
        {
            m_serviceWeak = service;
            return true;
        }
    }

    return false;
}

template <typename T>
std::shared_ptr<T> LazyServiceProvider<T>::GetLazy() const
{
    // check if service was acquired before
    if (!m_serviceWeak.owner_before(std::weak_ptr<T>{}) && !std::weak_ptr<T>{}.owner_before(m_serviceWeak))
    {
        if (!TryGetService())
        {
            return nullptr;  // service is not registered or locator dead
        }
    }

    if (auto service = m_serviceWeak.lock())
    {
        return service;  // found service
    }
    else
    {
        // service is dead, forget it for further reacquiring
        m_serviceWeak.reset();
        return nullptr;
    }
}

template <typename T>
void LazyServiceProvider<T>::SetLocator(std::weak_ptr<IServiceLocator> locator) const
{
    m_locatorWeak = std::move(locator);
}

}  // namespace tools
}  // namespace srv

#endif  // H_E0B79933_027A_4D3F_8C49_E78BA42A62BC
