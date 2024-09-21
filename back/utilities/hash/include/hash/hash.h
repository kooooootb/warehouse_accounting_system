#ifndef H_2931223A_E33C_4F9D_9AA1_2FBA7D31DFA6
#define H_2931223A_E33C_4F9D_9AA1_2FBA7D31DFA6

#include <string>

#include <instrumental/common.h>

namespace util
{
namespace hash
{

std::vector<uint8_t> Hash(std::string_view input);
std::string HashToBase64(std::string_view input);

}  // namespace hash

}  // namespace util

#endif  // H_2931223A_E33C_4F9D_9AA1_2FBA7D31DFA6