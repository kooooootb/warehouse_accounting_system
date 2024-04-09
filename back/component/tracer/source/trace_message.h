#ifndef H_D5354EB6_3EFA_4B65_B4C9_9FC01FAEDE22
#define H_D5354EB6_3EFA_4B65_B4C9_9FC01FAEDE22

#include <list>
#include <string>

#include <tracer/tracer.h>

namespace srv {
namespace tracer {
namespace detail {

class TraceMessage : public ITraceMessage {
public:
    TraceMessage() = default;

    void AddToBack(std::string data) override;
    void AddToFront(std::string data) override;
    std::string ToString() const override;

private:
    inline size_t GetMessageSize() const;

private:
    std::list<std::string> m_message;
};

}  // namespace detail
}  // namespace tracer
}  // namespace srv

#endif  // H_D5354EB6_3EFA_4B65_B4C9_9FC01FAEDE22