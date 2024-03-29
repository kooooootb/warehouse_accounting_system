#ifndef H_861C76CA_903E_4D2D_9ABE_5B7F7AD65141
#define H_861C76CA_903E_4D2D_9ABE_5B7F7AD65141

#include <memory>

#include <ifaces/service_locator.h>

namespace srv {

ufa::Result CreateServiceLocator(std::unique_ptr<srv::IServiceLocator>& object);

}

#endif  // H_861C76CA_903E_4D2D_9ABE_5B7F7AD65141