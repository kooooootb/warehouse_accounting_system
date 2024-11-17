#ifndef H_DAD22A6D_C7D0_40B1_9BB2_AA2F31463739
#define H_DAD22A6D_C7D0_40B1_9BB2_AA2F31463739

#include <string_view>

#include <instrumental/common.h>

#include <db_connector/accessor.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>
#include <tasks/common/user.h>

namespace taskmgr
{
namespace tasks
{

class GetUser : public BaseTask
{
public:
    GetUser(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetUser;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetUser(srv::IAccessor& accessor);

private:
    static constexpr std::string_view ID_KEY = "id";
    static constexpr std::string_view USER_ID_KEY = "user_id";
    static constexpr std::string_view LOGIN_KEY = "login";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";

private:
    User m_user;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_DAD22A6D_C7D0_40B1_9BB2_AA2F31463739