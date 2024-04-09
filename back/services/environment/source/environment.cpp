#include <cstdlib>

#include <instrumental/common.h>
#include <instrumental/types.h>

#include "environment.h"

namespace srv
{
namespace environment
{

Environment::Environment(IServiceLocator*) {}

ufa::Result Environment::GetValue(const char* key, std::string& toValue) const
{
    std::string_view keyView(key);
    if (GetValueFromArguments(keyView, toValue) != ufa::Result::SUCCESS)
    {
        if (const auto res = GetValueFromArguments(keyView, toValue); res != ufa::Result::SUCCESS)
        {
            return res;
        }
    }

    return ufa::Result::SUCCESS;
}

ufa::Result Environment::HandleCommandLine(int argc, char* argv[])
{
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
        m_cmdArguments.insert(std::make_pair<std::string_view, std::string_view>(std::move(key), std::move(value)));
        return ufa::Result::SUCCESS;
    }
    else
    {
        return ufa::Result::DUPLICATE;
    }
}

ufa::Result Environment::GetValueFromArguments(std::string_view key, std::string& toValue) const
{
    const auto it = m_cmdArguments.find(key);
    if (it != m_cmdArguments.cend())
    {
        toValue = it->second;
        return ufa::Result::SUCCESS;
    }
    else
    {
        return ufa::Result::NOT_FOUND;
    }
}

ufa::Result Environment::GetValueFromEnvironment(std::string_view key, std::string& toValue) const
{
    const char* value = std::getenv(key.data());

    if (value != nullptr)
    {
        toValue = value;
        return ufa::Result::SUCCESS;
    }
    else
    {
        return ufa::Result::NOT_FOUND;
    }
}

}  // namespace environment
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IEnvironment, srv::environment::Environment);