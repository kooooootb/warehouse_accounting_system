#ifndef H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85
#define H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85

#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>

#include "callback.h"
#include "task_info.h"

namespace taskmgr
{

struct ITaskManager : public ufa::IBase
{
    /**
     * @brief parse task from json string and pass it to queue for execution
     * @param taskInfo task parameters
     * @return ufa::Result 
     */
    virtual ufa::Result AddTask(TaskInfo&& taskInfo) = 0;

    static std::unique_ptr<ITaskManager> Create(std::shared_ptr<srv::IServiceLocator> locator);
};

}  // namespace taskmgr

#endif  // H_CC0C686D_89E2_4CB3_9DA6_C0C92EDCBF85