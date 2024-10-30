#ifndef H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8
#define H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8

#include <instrumental/types.h>
#include <locator/service.h>
#include <locator/service_locator.h>

#include "settings.h"
#include "transaction.h"

namespace srv
{

struct IAccessor : public srv::IService
{
    DECLARE_IID(0X67B00E33);

    virtual std::unique_ptr<db::ITransaction> CreateTransaction() = 0;

    static std::unique_ptr<IAccessor> Create(std::shared_ptr<srv::IServiceLocator> locator);
};

}  // namespace srv

#endif  // H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8