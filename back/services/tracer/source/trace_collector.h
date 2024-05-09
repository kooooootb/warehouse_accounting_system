#ifndef H_E6E0EFF7_B44F_46E4_9FA5_C963F2B7F390
#define H_E6E0EFF7_B44F_46E4_9FA5_C963F2B7F390

#include <tracer/tracer.h>

namespace srv
{
namespace tracer
{

class TraceCollector : public ITraceCollector
{
public:
    TraceCollector(std::shared_ptr<ITracer> tracer, std::unique_ptr<ITraceMessage> traceMessage);

    ~TraceCollector() noexcept;

    void AddMessage(std::string&& message) override;
    void AddMessage(const char* message) override;

private:
    std::shared_ptr<ITracer> m_tracer;
    std::unique_ptr<ITraceMessage> m_traceMessage;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_E6E0EFF7_B44F_46E4_9FA5_C963F2B7F390