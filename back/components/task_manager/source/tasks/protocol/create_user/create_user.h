#ifndef H_E51A91B2_1A6F_41C0_B592_6D15B6DCF543
#define H_E51A91B2_1A6F_41C0_B592_6D15B6DCF543

#include <string_view>

#include <authorizer/user_info.h>
#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include <tasks/base_task.h>

namespace taskmgr
{
namespace tasks
{

class CreateUser : public BaseTask
{
public:
    CreateUser(std::shared_ptr<srv::ITracer> tracer, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::CreateUser;
    }

protected:
    ufa::Result ExecuteInternal(const srv::IServiceLocator& locator, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view LOGIN_KEY = "login";
    static constexpr std::string_view PASSWORD_KEY = "password";
    static constexpr std::string_view USER_ID_KEY = "user_id";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";

private:
    srv::auth::UserInfo m_userInfo;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_E51A91B2_1A6F_41C0_B592_6D15B6DCF543
