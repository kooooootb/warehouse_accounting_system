#ifndef H_5C83B082_2804_47D3_A671_FBACF9D38FBA
#define H_5C83B082_2804_47D3_A671_FBACF9D38FBA

#include <chrono>
#include <string>

#include <instrumental/time.h>

#include <instrumental/types.h>
#include <locator/service.h>

namespace srv
{

struct IDateProvider : public srv::IService
{
    DECLARE_IID(0xEDE6FE29);  // 3991338537

    /**
    * @brief get number that goes up as time goes by
    */
    virtual timestamp_t GetTimestamp() const = 0;

    /**
     * @brief get string with format: hh:MM:ss.mmmmmm
     */
    virtual std::string GetTimeString() const = 0;

    /**
     * @brief get string with format: yyyy-MM-dd'T'HH:mm:ss
     */
    virtual std::string ToIsoTimeString(timestamp_t timestamp) const = 0;

    /**
     * @brief get timestamp_t from format: yyyy-MM-dd'T'HH:mm:ss, ERROR on wrong format
     */
    virtual ufa::Result FromIsoTimeString(std::string_view timeString, timestamp_t& result) const = 0;
};

}  // namespace srv

#endif  // H_5C83B082_2804_47D3_A671_FBACF9D38FBA