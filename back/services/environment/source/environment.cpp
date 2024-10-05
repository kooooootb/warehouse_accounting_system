#include <cstdlib>

#include <instrumental/common.h>
#include <instrumental/types.h>

#include "environment.h"

namespace srv
{
namespace environment
{

namespace
{

std::string ToString(int argc, char* argv[])
{
    std::vector<std::string_view> tempArgs;
    tempArgs.reserve(argc);

    for (size_t i = 0; i < argc; ++i)
    {
        tempArgs.emplace_back(argv[i]);
    }

    const int resultSize = std::accumulate(std::cbegin(tempArgs),
                               std::cend(tempArgs),
                               0,
                               [](size_t res, const auto& str) -> size_t
                               {
                                   return res + str.size();
                               }) +
                           (tempArgs.size() - 1);

    std::string result;
    result.reserve(resultSize);

    std::for_each(std::cbegin(tempArgs),
        std::prev(std::cend(tempArgs)),
        [&result](const auto& str) mutable -> void
        {
            constexpr std::string_view Separator = " ";

            result.append(str);
            result.append(Separator);
        });

    result.append(tempArgs.back());

    return result;
}

}  // namespace

Environment::Environment(const std::shared_ptr<IServiceLocator>& locator) : srv::tracer::TracerLazyProvider(locator)
{
    TRACE_INF << TRACE_HEADER;
}

ufa::Result Environment::GetValue(std::string_view key, std::string& toValue) const
{
    TRACE_INF << TRACE_HEADER << "Getting value for key: " << key;

    if (GetValueFromArguments(key, toValue) != ufa::Result::SUCCESS)
    {
        if (const auto res = GetValueFromEnvironment(key, toValue); res != ufa::Result::SUCCESS)
        {
            TRACE_WRN << TRACE_HEADER << "Getting value failed with reason: " << res;
            return res;
        }
    }

    TRACE_INF << TRACE_HEADER << "Got value: " << toValue;
    return ufa::Result::SUCCESS;
}

ufa::Result Environment::HandleCommandLine(int argc, char* argv[])
{
    TRACE_INF << TRACE_HEADER << "cmdline: " << ToString(argc, argv);
    argc -= 1;  // for executable name
    int i = 1;

    // loop through pairs
    while (argc > 1)
    {
        std::string_view key{argv[i]};
        std::string_view value{argv[i + 1]};

        FormatKey(key);
        if (auto res = AddParameter(key, value); res != ufa::Result::SUCCESS)
        {
            return res;
        }

        i += 2;
        argc -= 2;
    }

    if (argc > 0)
    {
        // something left over
        TRACE_WRN << TRACE_HEADER << "Cmd line has odd arg: " << argv[i];
        return ufa::Result::WRONG_FORMAT;
    }

    return ufa::Result::SUCCESS;
}

void Environment::FormatKey(std::string_view& key) const
{
    const char* skipping = "/-";
    key.remove_prefix(std::min(key.find_first_not_of(skipping), key.size()));
}

ufa::Result Environment::AddParameter(std::string_view key, std::string_view value)
{
    const auto it = m_cmdArguments.find(key);
    if (it == m_cmdArguments.cend())
    {
        TRACE_INF << TRACE_HEADER << "Inserting key: " << key << ", value: " << value;

        m_cmdArguments.insert(std::make_pair<std::string_view, std::string_view>(std::move(key), std::move(value)));
        return ufa::Result::SUCCESS;
    }
    else
    {
        TRACE_WRN << TRACE_HEADER << "Met duplicate, key: " << key << ", value: " << value;
        return ufa::Result::DUPLICATE;
    }
}

ufa::Result Environment::GetValueFromArguments(std::string_view key, std::string& toValue) const
{
    TRACE_INF << TRACE_HEADER << "key: " << key;

    const auto it = m_cmdArguments.find(key);
    if (it != m_cmdArguments.cend())
    {
        toValue = it->second;

        TRACE_INF << TRACE_HEADER << "Received value: " << toValue;
        return ufa::Result::SUCCESS;
    }
    else
    {
        TRACE_WRN << TRACE_HEADER << "Couldn't find key";
        return ufa::Result::NOT_FOUND;
    }
}

ufa::Result Environment::GetValueFromEnvironment(std::string_view key, std::string& toValue) const
{
    TRACE_INF << TRACE_HEADER << "key: " << key;

    const char* value = std::getenv(key.data());

    if (value != nullptr)
    {
        toValue = value;

        TRACE_INF << TRACE_HEADER << "Received value: " << toValue;
        return ufa::Result::SUCCESS;
    }
    else
    {
        TRACE_WRN << TRACE_HEADER << "Couldn't find key";
        return ufa::Result::NOT_FOUND;
    }
}

}  // namespace environment
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IEnvironment, srv::environment::Environment);