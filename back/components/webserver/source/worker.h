#ifndef H_08AEC84F_36D9_4B61_84A1_0CB23C3E8B26
#define H_08AEC84F_36D9_4B61_84A1_0CB23C3E8B26

#include <atomic>
#include <thread>

#include <tracer/tracer_provider.h>

namespace ws
{

class Worker
{
public:
    template <typename Func, typename... Args>
    Worker(Func func, Args... args) : m_thread(func(std::forward<Args...>(args...)))
    {
        m_isFinished = true;
    }

    inline bool IsFinished() const
    {
        return m_isFinished;
    }

private:
    std::thread m_thread;
    std::atomic<bool> m_isFinished = false;
};

}  // namespace ws

#endif  // H_08AEC84F_36D9_4B61_84A1_0CB23C3E8B26