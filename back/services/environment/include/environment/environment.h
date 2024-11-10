#ifndef H_3DDE5E33_68B4_48A9_8B42_B066B6A2F438
#define H_3DDE5E33_68B4_48A9_8B42_B066B6A2F438

#include <instrumental/common.h>
#include <locator/service.h>

namespace srv
{

struct IEnvironment : public srv::IService
{
    DECLARE_IID(0XC0918430);  // 3230762032

    /**
     * @brief retrieve value from environment for given key
     * @param key environmental variable name
     * @param toValue where to put variable's value
     * @return ufa::Result NOT_FOUND if variable with specified key cant be found, SUCCESS otherwise
     */
    virtual ufa::Result GetValue(std::string_view key, std::string& toValue) const = 0;

    /**
     * @brief handle argc and argv user parameters
     * @return SUCCESS on no error, DUPLICATE when key duplicated, WRONG_FORMAT on leftovers
     */
    virtual ufa::Result HandleCommandLine(int argc, char* argv[]) = 0;
};

}  // namespace srv

#endif  // H_3DDE5E33_68B4_48A9_8B42_B066B6A2F438