#include <instrumental/common.h>
#include <instrumental/types.h>
#include <task_manager/task.h>

#include "task.h"

namespace taskmgr
{

ufa::Result ITask::ParseTask(std::string_view target,
    std::string&& json,
    std::function<void(std::string&&)>&& callback,
    std::unique_ptr<ITask>& task)
{
    return ufa::Result::ERROR;
}

}  // namespace taskmgr
