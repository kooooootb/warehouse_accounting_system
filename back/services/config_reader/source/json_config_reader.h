#ifndef H_836DA1A3_A7E2_493F_96AE_F9B00296A9DA
#define H_836DA1A3_A7E2_493F_96AE_F9B00296A9DA

#include <filesystem>

#include <nlohmann/json.hpp>

#include <config_reader/config_reader.h>
#include <locator/service_locator.h>

namespace json = nlohmann;

namespace srv
{
namespace config_reader
{

/**
 * @brief Reads values in format: {"<key1>":{"<key2>":<value>}}
 */
class JsonConfigReader : public srv::IConfigReader
{
public:
    JsonConfigReader(IServiceLocator* locator);

    ufa::Result ReadValue(const std::vector<std::string_view>& keys, std::string& value) override;

private:
    json::json m_settingMap;
};

}  // namespace config_reader
}  // namespace srv

#endif  // H_836DA1A3_A7E2_493F_96AE_F9B00296A9DA
