#include <chrono>
#include <cstdint>
#include <ctime>

#include <instrumental/check.h>
#include <instrumental/string_converters.h>
#include <instrumental/time.h>
#include <instrumental/types.h>

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

// "yyyy-MM-dd'T'HH:mm:ss"
constexpr std::string_view Format = "%F'T'%T";

DateProvider::DateProvider(const std::shared_ptr<IServiceLocator>& locator)
{
    const auto currentTimeFromEpoch = chrono::system_clock::now().time_since_epoch();
    const auto currentTimeHighresolution = chrono::high_resolution_clock::now().time_since_epoch();

    m_epochOffset = currentTimeFromEpoch - currentTimeHighresolution;

    // this might be cruel but goes only ones at start and does the job
    auto original = GetTimestamp();
    timestamp_t reverted;
    CHECK_SUCCESS(FromIsoTimeString(ToIsoTimeString(original), reverted));
    m_fromIsoOffset = reverted - original;
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

    std::strftime(result.data(), result.size(), Format.data(), std::gmtime(&timeT));

    result.resize(result.size() - 1);  // strftime requires 1 extra byte for \0 so we strip it here
    return result;
}

ufa::Result DateProvider::FromIsoTimeString(std::string_view timeString, timestamp_t& result) const
{
    try
    {
        constexpr std::string_view Example = "yyyy-MM-dd'T'HH:mm:ss";
        CHECK_TRUE(timeString.size() == Example.size() && timeString[4] == '-' && timeString[7] == '-' && timeString[10] == '\'' &&
                   timeString[11] == 'T' && timeString[12] == '\'' && timeString[15] == ':' && timeString[18] == ':');

        // dirty
        uint32_t years = string_converters::FromString<uint32_t>(std::string(timeString.substr(0, 4)));
        uint32_t monthes = string_converters::FromString<uint32_t>(std::string(timeString.substr(5, 2)));
        uint32_t days = string_converters::FromString<uint32_t>(std::string(timeString.substr(8, 2)));
        uint32_t hours = string_converters::FromString<uint32_t>(std::string(timeString.substr(13, 2)));
        uint32_t minutes = string_converters::FromString<uint32_t>(std::string(timeString.substr(16, 2)));
        uint32_t seconds = string_converters::FromString<uint32_t>(std::string(timeString.substr(19, 2)));

        std::tm tm{};

        CHECK_TRUE(monthes > 0 && monthes <= 12 && days > 0 && days <= 31 && hours > 0 && hours <= 24 && minutes > 0 &&
                   minutes <= 60 && seconds > 0 && seconds <= 60);

        tm.tm_year = years - 1900;  // since january
        tm.tm_mon = monthes - 1;    // [0, 11]
        tm.tm_mday = days;
        tm.tm_hour = hours;
        tm.tm_min = minutes;
        tm.tm_sec = seconds;

        auto timeT = std::mktime(&tm);
        CHECK_TRUE(timeT != -1);

        auto systemClock = chrono::system_clock::from_time_t(timeT).time_since_epoch();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(systemClock);
        result = duration.count() - m_fromIsoOffset;

        return ufa::Result::SUCCESS;
    }
    catch (const std::exception& ex)
    {
        return ufa::Result::ERROR;
    }
}

}  // namespace date
}  // namespace srv

DECLARE_DEFAULT_INTERFACE(srv::IDateProvider, srv::date::DateProvider);