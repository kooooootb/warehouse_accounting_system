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

/**
 * @brief Specify trace message to process and save.
 * Bigger number means more memory and processing consumption and more detailed saved messages
 */
enum class TraceLevel : int32_t {
    DISABLED = 0,    // no tracing
    ALWAYS = 200,    // message absolutely important for logging
    CRITICAL = 400,  // errors in critical components only
    ERROR = 600,     // errors in common components
    WARNING = 800,   // least significant error messages and important rare scenarios
    INFO = 1000,     // common information
    DEBUG = 1200,    // same as INFO but with big messages that may affect the performance
};

}  // namespace ufa

#endif  // H_BD52BD5A_F72A_4609_96CB_58F69390593C