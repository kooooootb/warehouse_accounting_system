#ifndef H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8
#define H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8

#include <db_connector/data/user.h>
#include <instrumental/interface.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>

namespace db
{

struct IAccessor : public ufa::IBase
{
    virtual ufa::Result FillUser(data::User& user) = 0;

    static std::unique_ptr<IAccessor> Create(std::shared_ptr<srv::IServiceLocator> locator);
};

}  // namespace db

#endif  // H_867F46FF_9C68_4498_BE3C_C4F5DF2520C8