#ifndef H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85
#define H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85

#include <authorizer/authorizer.h>
#include <db_connector/accessor.h>
#include <instrumental/interface.h>
#include <locator/service_locator.h>

#include "task.h"

namespace taskmgr
{

struct ITaskManager : public ufa::IBase
{
    virtual void AddTask(std::unique_ptr<ITask>&& task) = 0;

    static std::unique_ptr<ITaskManager> Create(std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<db::IAccessor> accessor,
        std::shared_ptr<auth::IAuthorizer> authorizer);
};

}  // namespace taskmgr

#endif  // H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85