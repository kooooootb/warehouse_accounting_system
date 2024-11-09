#include <filesystem>
#include <fstream>
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

namespace
{

/**
 * @brief pretty print vector for informational purposes
 */
inline std::string ToString(const std::vector<std::string_view>& from)
{
    if (from.empty())
    {
        return {};
    }

    const int resultSize = std::accumulate(std::cbegin(from),
                               std::cend(from),
                               0,
                               [](size_t res, const auto& str) -> size_t
                               {
                                   return res + str.size();
                               }) +
                           (from.size() - 1) * 2 + 2;

    std::string result;
    result.push_back('{');
    result.reserve(resultSize);

    std::for_each(std::cbegin(from),
        std::prev(std::cend(from)),
        [&result](const auto& str) mutable -> void
        {
            constexpr std::string_view Separator = ", ";

            result.append(str);
            result.append(Separator);
        });

    result.append(from.back());
    result.push_back('}');

    return result;
}

}  // namespace

JsonConfigReader::JsonConfigReader(const std::shared_ptr<IServiceLocator>& locator) : srv::tracer::TracerLazyProvider(locator)
{
    TRACE_INF << TRACE_HEADER;

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
        try
        {
            m_settingMap = json::json::parse(ifs);
            CHECK_TRUE(!m_settingMap.is_discarded());
            TRACE_DBG << TRACE_HEADER << "Read json config: " << m_settingMap.dump();
        }
        catch (const std::exception& ex)
        {
            TRACE_ERR << TRACE_HEADER << "Couldn't read json config, path: " << configPath.string();
        }
    }
    else
    {
        TRACE_ERR << TRACE_HEADER << "Config file couldn't be opened, path: " << configPath.string();
        m_settingMap.clear();
    }
}

ufa::Result JsonConfigReader::ReadValue(const std::vector<std::string_view>& keys, std::string& value)
{
    TRACE_DBG << TRACE_HEADER << ToString(keys);

    if (m_settingMap.empty())
    {
        TRACE_ERR << TRACE_HEADER << "Json map not initialized";
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

        TRACE_DBG << TRACE_HEADER << "Read value: " << value;
        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
    }

    TRACE_DBG << TRACE_HEADER << "Not found";
    return ufa::Result::NOT_FOUND;
}

}  // namespace config_reader
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IConfigReader, srv::config_reader::JsonConfigReader);
