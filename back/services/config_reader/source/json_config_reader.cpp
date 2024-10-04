#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <environment/environment.h>
#include <instrumental/check.h>
#include <instrumental/common.h>
#include <instrumental/types.h>

#include "json_config_reader.h"

namespace srv
{
namespace config_reader
{

JsonConfigReader::JsonConfigReader(IServiceLocator* locator)
{
    std::shared_ptr<srv::IEnvironment> environment;
    CHECK_SUCCESS(locator->GetInterface(environment));

    std::string configPathStr;
    std::filesystem::path configPath;
    const auto configFound = environment->GetValue(CONFIG_ENV_KEY, configPathStr);

    if (configFound != ufa::Result::SUCCESS)
    {
        configPath = std::filesystem::current_path() / CONFIG_DEFAULT_FILE;
    }
    else
    {
        configPath = std::move(configPathStr);
    }

    std::ifstream ifs;
    ifs.open(configPath);

    if (ifs.good())
    {
        m_settingMap = json::json::parse(ifs);
    }
    else
    {
        m_settingMap.clear();
    }
}

ufa::Result JsonConfigReader::ReadValue(const std::vector<std::string_view>& keys, std::string& value)
{
    if (m_settingMap.empty())
    {
        return ufa::Result::NOT_FOUND;
    }

    try
    {
        const json::json* it = &m_settingMap;

        for (const auto& key : keys)
        {
            it = &it->at(key);
        }

        CHECK_TRUE(!it->is_object() || !it->is_array());

        if (it->is_string())
        {
            value = it->get<std::string>();  // dont wrap it in " symbols
        }
        else
        {
            value = it->dump();
        }

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
    }

    return ufa::Result::NOT_FOUND;
}

}  // namespace config_reader
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IConfigReader, srv::config_reader::JsonConfigReader);
