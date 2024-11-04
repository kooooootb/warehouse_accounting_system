#ifndef H_4D76E0A4_463B_457C_9937_D9847B147E59
#define H_4D76E0A4_463B_457C_9937_D9847B147E59

#include <instrumental/common.h>

namespace ws
{
namespace session
{

class ISession : public ufa::IBase
{
public:
    virtual void Run() = 0;
};

}  // namespace session
}  // namespace ws

#endif  // H_4D76E0A4_463B_457C_9937_D9847B147E59