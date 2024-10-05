#ifndef H_935D41D5_F822_40DD_917B_38B905BCDDD3
#define H_935D41D5_F822_40DD_917B_38B905BCDDD3

#include <chrono>

#include <date_provider/date_provider.h>

#include <locator/service_locator.h>
#include <tracer/tracer_lazy_provider.h>

namespace srv
{
namespace date
{

// doesn't have tracerprovider as tracer is getting timestamps with this service which will start infinite loop
class DateProvider : public IDateProvider
{
public:
    DateProvider(const std::shared_ptr<IServiceLocator>& locator);

    uint64_t GetTimestamp() const override;
    std::string GetTimeString() const override;

private:
    std::chrono::high_resolution_clock::duration GetDuration() const;

private:
    std::chrono::high_resolution_clock::duration m_epochOffset;
};

}  // namespace date
}  // namespace srv

#endif  // H_935D41D5_F822_40DD_917B_38B905BCDDD3