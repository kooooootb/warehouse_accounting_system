#ifndef H_57C6E732_5A33_4CE6_8DAF_65613C3B4811
#define H_57C6E732_5A33_4CE6_8DAF_65613C3B4811

#include <string_view>

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

class Authorization : public BaseTask
{
public:
    Authorization(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::Authorization;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view LOGIN_KEY = "login";
    static constexpr std::string_view PASSWORD_KEY = "password";
    static constexpr std::string_view TOKEN_KEY = "token";
    static constexpr std::string_view USERID_KEY = "user_id";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";

private:
    std::string m_login;
    std::string m_hashPassword;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_57C6E732_5A33_4CE6_8DAF_65613C3B4811