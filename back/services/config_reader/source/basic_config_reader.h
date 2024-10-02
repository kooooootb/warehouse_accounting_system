#ifndef H_F19B4492_7CE5_4601_908D_48E6E0139F81
#define H_F19B4492_7CE5_4601_908D_48E6E0139F81

#include <filesystem>

#include <config_reader/config_reader.h>
#include <locator/service_locator.h>

namespace srv
{
namespace config_reader
{

constexpr char SPLITTER = '=';
constexpr char COMBINER = '_';

class BasicConfigReader : public srv::IConfigReader
{
public:
    BasicConfigReader(IServiceLocator* locator);

    ufa::Result ReadValue(std::vector<std::string_view> keys, std::string& value) override;

private:
    std::pair<std::string_view, std::string_view> SplitLine(std::string_view);

private:
    std::filesystem::path m_configPath;
};

}  // namespace config_reader
}  // namespace srv

#endif  // H_F19B4492_7CE5_4601_908D_48E6E0139F81