#ifndef H_BD52BD5A_F72A_4609_96CB_58F69390593C
#define H_BD52BD5A_F72A_4609_96CB_58F69390593C

#include <cstdint>

namespace ufa {

using iid_t = int32_t;

enum class Result : int32_t {
    SUCCESS = 0,                    // no error
    ERROR = 1,                      // general error, dont use if more suitable error exists
    NO_INTERFACE = 800,             // if no such interface available
    REREGISTERING_INTERFACE = 801,  // if trying to reregister interface
};

}  // namespace ufa

#endif  // H_BD52BD5A_F72A_4609_96CB_58F69390593C