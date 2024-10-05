#ifndef H_D026DFC6_0149_4FDF_AC84_D76A89471021
#define H_D026DFC6_0149_4FDF_AC84_D76A89471021

#include "tracer.h"

namespace srv
{
namespace tracer
{

class TracerProvider
{
public:
    TracerProvider(std::shared_ptr<ITracer> tracer) : m_tracer(std::move(tracer)) {}

protected:
    // return ref to shared pointer as provider owns it already
    // but users can pass tracer to other objects
    const std::shared_ptr<ITracer>& GetTracer()
    {
        return m_tracer;
    }

private:
    std::shared_ptr<ITracer> m_tracer;
};

}  // namespace tracer
}  // namespace srv

#endif  // H_D026DFC6_0149_4FDF_AC84_D76A89471021