#ifndef H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85
#define H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85

#include <authorizer/authorizer.h>
#include <db_connector/accessor.h>
#include <instrumental/interface.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>

#include <task_manager/callback.h>

namespace taskmgr
{

struct ITaskManager : public ufa::IBase
{
    /**
     * @brief parse task from json string and pass it to queue for execution
     * @param target task identificator
     * @param callback will call it after gettings result
     * @return ufa::Result 
     */
    virtual ufa::Result AddTask(std::string_view target, std::string&& json, Callback&& callback) = 0;

    static std::unique_ptr<ITaskManager> Create(std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<db::IAccessor> accessor,
        std::shared_ptr<auth::IAuthorizer> authorizer);
};

}  // namespace taskmgr

#endif  // H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85