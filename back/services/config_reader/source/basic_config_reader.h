#ifndef H_F19B4492_7CE5_4601_908D_48E6E0139F81
#define H_F19B4492_7CE5_4601_908D_48E6E0139F81

#include <filesystem>

#include <config_reader/config_reader.h>
#include <locator/service_locator.h>
#include <tracer/tracer_lazy_provider.h>

namespace srv
{
namespace config_reader
{

constexpr char SPLITTER = '=';
constexpr char COMBINER = '_';

/**
 * @brief Reads values in format: <key1>_<key2>=<value>
 */
class BasicConfigReader : public srv::tracer::TracerLazyProvider, public srv::IConfigReader
{
public:
    BasicConfigReader(const std::shared_ptr<IServiceLocator>& locator);

    ufa::Result ReadValue(const std::vector<std::string_view>& keys, std::string& value) override;

private:
    std::pair<std::string_view, std::string_view> SplitLine(std::string_view);

private:
    std::filesystem::path m_configPath;
};

}  // namespace config_reader
}  // namespace srv

#endif  // H_F19B4492_7CE5_4601_908D_48E6E0139F81