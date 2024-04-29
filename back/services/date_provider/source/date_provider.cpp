#include <chrono>

#include "date_provider.h"

namespace srv
{
namespace date
{

namespace chrono = std::chrono;

DateProvider::DateProvider(IServiceLocator* serviceLocator)
{
    const auto currentTimeFromEpoch = chrono::system_clock::now().time_since_epoch();
    const auto currentTimeHighresolution = chrono::high_resolution_clock::now().time_since_epoch();

    m_epochOffset = currentTimeHighresolution - currentTimeFromEpoch;
}

std::chrono::high_resolution_clock::duration DateProvider::GetDuration() const
{
    return chrono::high_resolution_clock::now().time_since_epoch() + m_epochOffset;
}

uint64_t DateProvider::GetTimestamp() const
{
    return chrono::duration_cast<chrono::nanoseconds>(GetDuration()).count();
}

std::string DateProvider::GetTimeString() const
{
    // TODO: strftime
    const auto duration = GetDuration();

    std::string result(12, '0');

    auto hours = std::to_string(chrono::duration_cast<chrono::hours>(duration).count() % 24);
    auto minutes = std::to_string(chrono::duration_cast<chrono::minutes>(duration).count() % 60);
    auto seconds = std::to_string(chrono::duration_cast<chrono::seconds>(duration).count() % 60);
    auto milliseconds = std::to_string(chrono::duration_cast<chrono::milliseconds>(duration).count() % 1000);

    hours.resize(2, '0');
    minutes.resize(2, '0');
    seconds.resize(2, '0');
    milliseconds.resize(3, '0');

    result.insert(0, hours.c_str(), 2);
    result[2] = ':';
    result.insert(3, minutes.c_str(), 2);
    result[5] = ':';
    result.insert(6, seconds.c_str(), 2);
    result[8] = '.';
    result.insert(9, milliseconds.c_str(), 3);

    return result;
}

}  // namespace date
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IDateProvider, srv::date::DateProvider);