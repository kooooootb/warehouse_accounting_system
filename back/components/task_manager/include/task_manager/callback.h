#ifndef H_E55BFED7_B63E_4D47_BD87_023EA5F72030
#define H_E55BFED7_B63E_4D47_BD87_023EA5F72030

#include <instrumental/types.h>

namespace taskmgr
{

using callback_t = std::function<void(std::string&& /* result */, ufa::Result /* error */)>;

}  // namespace taskmgr

#endif  // H_E55BFED7_B63E_4D47_BD87_023EA5F72030