#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>

#include <instrumental/check.h>
#include <instrumental/common.h>

#include "back/services/config_reader/include/settings_provider/config_reader.h"
#include "basic_config_reader.h"

namespace srv
{
namespace config_reader
{

BasicConfigReader::BasicConfigReader(IServiceLocator* locator)
{
    const std::shared_ptr<srv::IEnvironment> environment;
    CHECK_SUCCESS(locator->GetInterface(environment));

    const auto configFound = environment->GetValue(CONFIG_ENV_KEY, m_configPath);

    if (configFound != ufa::Result::SUCCESS)
    {
        m_configPath = std::filesystem::current_path() / CONFIG_DEFAULT_FILE;
    }
}

ufa::Result BasicConfigReader::ReadValue(std::string_view key, std::string& value)
{
    std::ifstream fs(m_configPath);

    std::string line;
    while (std::getline(fs, line))
    {
        const auto [curKey, curValue] = SplitLine(line);
        if (curKey == key)
        {
            value = curValue;
            return ufa::Result::SUCCESS;
        }
    }

    return ufa::Result::NOT_FOUND;
}

std::pair<std::string_view, std::string_view> BasicConfigReader::SplitLine(std::string_view line)
{
    const auto splitterPos = line.find(SPLITTER);

    if (splitterPos == line.npos)
    {
        CHECK_SUCCESS(ufa::Result::WRONG_FORMAT, "met wrong formatted config line: " << line);
    }
    else
    {
        return {line.substr(0, splitterPos), line.substr(splitterPos + 1, line.length())};
    }
}

}  // namespace config_reader
}  // namespace srv