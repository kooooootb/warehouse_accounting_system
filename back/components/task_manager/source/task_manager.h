#ifndef H_528C9648_F7EE_46C5_85B7_E6C04E6A4D6E
#define H_528C9648_F7EE_46C5_85B7_E6C04E6A4D6E

#include <authorizer/authorizer.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer_provider.h>

namespace taskmgr
{

class TaskManager : public srv::tracer::TracerProvider, public ITaskManager
{
public:
    TaskManager(std::shared_ptr<srv::IServiceLocator> locator,
        std::shared_ptr<db::IAccessor> accessor,
        std::shared_ptr<auth::IAuthorizer> authorizer);

    ~TaskManager() noexcept override;

    void AddTask(std::unique_ptr<ITask>&& task) override;
};

}  // namespace taskmgr

#endif  // H_528C9648_F7EE_46C5_85B7_E6C04E6A4D6E