#include "config_reader.h"

#include <fstream>
#include <stdexcept>

#include <instrumental/common.h>

namespace srv
{
namespace settings_provider
{

ConfigReader::ConfigReader(std::filesystem::path configPath) : m_configPath(std::move(configPath)) {}

ufa::Result ConfigReader::ReadValue(std::string_view key, std::string& value)
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

std::pair<std::string_view, std::string_view> ConfigReader::SplitLine(std::string_view line)
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

}  // namespace settings_provider
}  // namespace srv