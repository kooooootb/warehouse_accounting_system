#ifndef H_8742E911_3E9D_489C_9BF3_BDD4A1B517E2
#define H_8742E911_3E9D_489C_9BF3_BDD4A1B517E2

#include <environment/environment.h>
#include <locator/service_locator.h>

namespace srv
{
namespace environment
{

class Environment : public IEnvironment
{
public:
    Environment(IServiceLocator*);

    ufa::Result GetValue(const char* key, std::string& toValue) const override;
};

}  // namespace environment
}  // namespace srv

#endif  // H_8742E911_3E9D_489C_9BF3_BDD4A1B517E2