#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <environment/environment.h>
#include <instrumental/check.h>
#include <instrumental/common.h>

#include "basic_config_reader.h"

namespace srv
{
namespace config_reader
{

namespace
{

std::string CombineKeys(const std::vector<std::string_view>& keys)
{
    std::string result;

    size_t resultSize = keys.size() - 1;
    for (const auto& key : keys)
    {
        resultSize += key.size();
    }

    result.reserve(resultSize);

    for (size_t i = 0; i < keys.size() - 1; ++i)
    {
        result.append(keys[i]);
        result.push_back(COMBINER);
    }
    result.append(keys.back());

    return result;
}

}  // namespace

BasicConfigReader::BasicConfigReader(IServiceLocator* locator)
{
    std::shared_ptr<srv::IEnvironment> environment;
    CHECK_SUCCESS(locator->GetInterface(environment));

    std::string configPathStr;
    const auto configFound = environment->GetValue(CONFIG_ENV_KEY, configPathStr);

    if (configFound != ufa::Result::SUCCESS)
    {
        m_configPath = std::filesystem::current_path() / CONFIG_DEFAULT_FILE;
    }
    else
    {
        m_configPath = std::move(configPathStr);
    }
}

ufa::Result BasicConfigReader::ReadValue(const std::vector<std::string_view>& keys, std::string& value)
{
    std::ifstream fs(m_configPath);
    const std::string key = CombineKeys(keys);

    std::string line;
    while (std::getline(fs, line))
    {
        try
        {
            const auto [curKey, curValue] = SplitLine(line);
            if (curKey == key)
            {
                value = curValue;
                return ufa::Result::SUCCESS;
            }
        }
        catch (const std::exception& ex)
        {
            std::cout << ex.what() << std::endl;
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

// DECLARE_DEFAULT_INTERFACE(srv::IConfigReader, srv::config_reader::BasicConfigReader);