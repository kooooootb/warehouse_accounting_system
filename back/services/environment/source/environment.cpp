#include <cstdlib>

#include <instrumental/common.h>

#include "environment.h"

namespace srv
{
namespace environment
{

Environment::Environment(IServiceLocator*) {}

ufa::Result Environment::GetValue(const char* key, std::string& toValue) const
{
    const char* value = std::getenv(key);

    if (value != nullptr)
    {
        toValue = value;
        return ufa::Result::SUCCESS;
    }
    else
    {
        return ufa::Result::NOT_FOUND;
    }
}

}  // namespace environment
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IEnvironment, srv::environment::Environment);