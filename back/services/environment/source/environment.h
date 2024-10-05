#ifndef H_8742E911_3E9D_489C_9BF3_BDD4A1B517E2
#define H_8742E911_3E9D_489C_9BF3_BDD4A1B517E2

#include <environment/environment.h>
#include <locator/service_locator.h>
#include <tracer/tracer_lazy_provider.h>

namespace srv
{
namespace environment
{

class Environment : public srv::tracer::TracerLazyProvider, public IEnvironment
{
public:
    Environment(const std::shared_ptr<IServiceLocator>& locator);

    ufa::Result GetValue(std::string_view key, std::string& toValue) const override;
    ufa::Result HandleCommandLine(int argc, char* argv[]) override;

private:
    /**
    * @brief Skip dashes, slashes in front
    */
    void FormatKey(std::string_view& key) const;
    ufa::Result AddParameter(std::string_view key, std::string_view value);

    ufa::Result GetValueFromArguments(std::string_view key, std::string& toValue) const;
    ufa::Result GetValueFromEnvironment(std::string_view key, std::string& toValue) const;

private:
    std::unordered_map<std::string_view, std::string_view> m_cmdArguments;
};

}  // namespace environment
}  // namespace srv

#endif  // H_8742E911_3E9D_489C_9BF3_BDD4A1B517E2