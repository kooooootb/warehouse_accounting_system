#ifndef H_01792A4D_E202_49EA_AE88_ACCB974A085F
#define H_01792A4D_E202_49EA_AE88_ACCB974A085F

#include <gmock/gmock.h>

#include <tracer/trace_level.h>
#include <tracer/tracer.h>

namespace srv
{
namespace mock
{

struct TracerMock : public ITracer
{
    MOCK_METHOD(tracer::TraceCollectorProxy, StartCollecting, (tracer::TraceLevel traceLevel));
    MOCK_METHOD(void, SetSettings, (const tracer::TracerSettings& settings));
    MOCK_METHOD(void, Trace, (std::unique_ptr<tracer::ITraceMessage> traceMessage));

    static std::shared_ptr<TracerMock> Create()
    {
        using namespace testing;

        auto tracer = std::make_shared<srv::mock::TracerMock>();

        EXPECT_CALL(*tracer, StartCollecting(_))
            .WillRepeatedly(Invoke(
                [](tracer::TraceLevel traceLevel)
                {
                    tracer::TraceCollectorProxy proxy(nullptr);
                    return proxy;
                }));

        return tracer;
    }
};

}  // namespace mock
}  // namespace srv

#endif  // H_01792A4D_E202_49EA_AE88_ACCB974A085F