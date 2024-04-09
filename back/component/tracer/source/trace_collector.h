#ifndef H_E6E0EFF7_B44F_46E4_9FA5_C963F2B7F390
#define H_E6E0EFF7_B44F_46E4_9FA5_C963F2B7F390

#include <tracer/tracer.h>

namespace srv {
namespace tracer {
namespace detail {

class TraceCollector : public ITraceCollector {
public:
    TraceCollector(TraceLevel traceLevel, std::shared_ptr<ITracer> tracer);

    ~TraceCollector() noexcept;

    void AddMessage(std::string&& message) override;
    void AddMessage(const char* message) override;

private:
    TraceLevel m_traceLevel;
    std::shared_ptr<ITracer> m_tracer;
    std::unique_ptr<ITraceMessage> m_traceMessage;
};

}  // namespace detail
}  // namespace tracer
}  // namespace srv

#endif  // H_E6E0EFF7_B44F_46E4_9FA5_C963F2B7F390