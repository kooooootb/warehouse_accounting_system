#ifndef H_F19B4492_7CE5_4601_908D_48E6E0139F81
#define H_F19B4492_7CE5_4601_908D_48E6E0139F81

#include <filesystem>

#include <instrumental/common.h>

namespace srv
{
namespace settings_provider
{

constexpr char SPLITTER = '=';

class ConfigReader
{
public:
    ConfigReader(std::filesystem::path configPath);

    ufa::Result ReadValue(std::string_view key, std::string& value);

private:
    std::pair<std::string_view, std::string_view> SplitLine(std::string_view);

private:
    std::filesystem::path m_configPath;
};

}  // namespace settings_provider
}  // namespace srv

#endif  // H_F19B4492_7CE5_4601_908D_48E6E0139F81