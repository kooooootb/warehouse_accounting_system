#ifndef H_2946B933_EB7F_440E_9F83_BA0DB2F6B4D9
#define H_2946B933_EB7F_440E_9F83_BA0DB2F6B4D9

#include <string_view>

#include <instrumental/common.h>

#include <authorizer/user_info.h>
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

class GetCurrentUser : public BaseTask
{
public:
    GetCurrentUser(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetCurrentUser;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetCurrentUser(srv::IAccessor& accessor);

private:
    static constexpr std::string_view USER_ID_KEY = "user_id";
    static constexpr std::string_view LOGIN_KEY = "login";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view RESULT_KEY = "result";

private:
    srv::auth::UserInfo m_user;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_2946B933_EB7F_440E_9F83_BA0DB2F6B4D9