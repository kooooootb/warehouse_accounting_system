#include <chrono>
#include <ctime>

#include "date_provider.h"

namespace srv
{
namespace date
{

namespace
{

void PadWithZeroes(std::string& value, std::string::size_type resultWidth)
{
    value = std::string(resultWidth - std::min(resultWidth, value.length()), '0') + value;
}

template <typename DurationT>
DurationT Extract(std::chrono::nanoseconds& duration)
{
    const auto result = std::chrono::duration_cast<DurationT>(duration);
    duration -= result;
    return result;
}

}  // namespace

namespace chrono = std::chrono;

DateProvider::DateProvider(const std::shared_ptr<IServiceLocator>& locator)
{
    const auto currentTimeFromEpoch = chrono::system_clock::now().time_since_epoch();
    const auto currentTimeHighresolution = chrono::high_resolution_clock::now().time_since_epoch();

    m_epochOffset = currentTimeFromEpoch - currentTimeHighresolution;
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

    std::string result;
    result.reserve(12);

    auto hours = std::to_string(chrono::duration_cast<chrono::hours>(duration).count() % 24);
    auto minutes = std::to_string(chrono::duration_cast<chrono::minutes>(duration).count() % 60);
    auto seconds = std::to_string(chrono::duration_cast<chrono::seconds>(duration).count() % 60);
    auto microseconds = std::to_string(chrono::duration_cast<chrono::microseconds>(duration).count() % 1000000);

    PadWithZeroes(hours, 2);
    PadWithZeroes(minutes, 2);
    PadWithZeroes(seconds, 2);
    PadWithZeroes(microseconds, 6);

    result.insert(0, hours.c_str(), 2);
    result.push_back(':');
    result.insert(3, minutes.c_str(), 2);
    result.push_back(':');
    result.insert(6, seconds.c_str(), 2);
    result.push_back('.');
    result.insert(9, microseconds.c_str(), 6);

    return result;
}

std::string DateProvider::ToIsoTimeString(timestamp_t timestamp) const
{
    auto duration = chrono::nanoseconds(timestamp);
    chrono::system_clock::time_point timePoint(chrono::duration_cast<chrono::system_clock::duration>(duration));
    std::time_t timeT = chrono::system_clock::to_time_t(timePoint);

    std::string result;
    result.resize(22);

    // "yyyy-MM-dd'T'HH:mm:ss"
    constexpr std::string_view Format = "%F'T'%T";

    std::strftime(result.data(), result.size(), Format.data(), std::gmtime(&timeT));

    result.resize(result.size() - 1);  // strftime requires 1 extra byte for \0 so we strip it here
    return result;
}

}  // namespace date
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IDateProvider, srv::date::DateProvider);