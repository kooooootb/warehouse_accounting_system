#include <numeric>

#include "trace_message.h"

namespace srv {
namespace tracer {

void TraceMessage::AddToBack(std::string data)
{
    m_message.emplace_back(std::move(data));
}

void TraceMessage::AddToFront(std::string data)
{
    m_message.emplace_front(std::move(data));
}

size_t TraceMessage::GetMessageSize() const
{
    return std::accumulate(std::cbegin(m_message), std::cend(m_message), 0);
}

std::string TraceMessage::ToString() const
{
    std::string result;
    result.reserve(GetMessageSize());

    size_t i = 0;
    for (const auto& string : m_message) {
        result.append(string);
    }

    return result;
}

}  // namespace tracer
}  // namespace srv
