#include <numeric>

#include <instrumental/string_converters.h>

#include "trace_message.h"

namespace srv
{
namespace tracer
{

TraceMessage::TraceMessage(index_t index, TraceLevel traceLevel, std::shared_ptr<IDateProvider> dateProvider)
    : m_traceLevel(traceLevel)
{
    AddToBack(std::to_string(index));
    AddToBack("\t");
    AddToBack(dateProvider->GetTimeString());
    AddToBack("\t");
    AddToBack(string_converters::ToString(m_traceLevel));
    AddToBack("\t");
}

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
    return std::accumulate(std::cbegin(m_message),
        std::cend(m_message),
        0,
        [](const auto& size, const auto& m)
        {
            return m.size() + size;
        });
}

std::string TraceMessage::ToString() const
{
    std::string result;
    result.reserve(GetMessageSize() + 1);  // 1 for \n

    for (const auto& string : m_message)
    {
        result.append(string);
    }
    result.append("\n");

    return result;
}

TraceLevel TraceMessage::GetTraceLevel() const
{
    return m_traceLevel;
}

}  // namespace tracer
}  // namespace srv
