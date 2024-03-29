#ifndef H_279EA6B0_A913_4430_A483_0A5FAAA2F146
#define H_279EA6B0_A913_4430_A483_0A5FAAA2F146

#include <string>

#include <instrumental/interface.h>

namespace srv {

/**
 * @brief effectively save logs while executing for debugging
 */
struct ITracer : public ufa::IBase {
    DECLARE_IID(0x37787C77)

    /**
     * @brief Collect whole message and process it on destruction
     */
    struct TraceCollector;

    virtual TraceCollector Trace(ufa::TraceLevel traceLevel) = 0;
};

}  // namespace srv

#endif  // H_279EA6B0_A913_4430_A483_0A5FAAA2F146