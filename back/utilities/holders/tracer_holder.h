#ifndef H_956BD391_DF1E_4EEB_A338_B9FA109E57DC
#define H_956BD391_DF1E_4EEB_A338_B9FA109E57DC

#include <ifaces/tracer.h>

class TracerHolder {
public:
    TracerHolder(srv::ITracer* tracer) : m_tracer(tracer) {}

protected:
    srv::ITracer* GetTracer() { return m_tracer; }

private:
    srv::ITracer* m_tracer;
};

#endif  // H_956BD391_DF1E_4EEB_A338_B9FA109E57DC