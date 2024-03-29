#ifndef H_D5354EB6_3EFA_4B65_B4C9_9FC01FAEDE22
#define H_D5354EB6_3EFA_4B65_B4C9_9FC01FAEDE22

#include <list>
#include <string>

namespace srv {
namespace tracer {

class TraceMessage {
public:
    TraceMessage() = default;

    void AddToBack(std::string);
    void AddToFront(std::string);

    std::string ToString() const;

private:
    size_t GetMessageSize() const;

private:
    std::list<std::string> m_message;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_D5354EB6_3EFA_4B65_B4C9_9FC01FAEDE22