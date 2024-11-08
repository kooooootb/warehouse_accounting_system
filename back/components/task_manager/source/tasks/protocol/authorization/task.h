#ifndef H_57C6E732_5A33_4CE6_8DAF_65613C3B4811
#define H_57C6E732_5A33_4CE6_8DAF_65613C3B4811

#include <string_view>

#include <authorizer/authorizer.h>
#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>

#include <task_manager/callback.h>
#include <tasks/base_task.h>

namespace taskmgr
{
namespace tasks
{

class Authorization : public BaseTask
{
public:
    Authorization(std::shared_ptr<srv::ITracer> tracer, srv::auth::userid_t userId, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "authorization";
    }

protected:
    ufa::Result ExecuteInternal(const srv::IServiceLocator& locator, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view USERNAME_KEY = "login";
    static constexpr std::string_view PASSWORD_KEY = "password";
    static constexpr std::string_view TOKEN_KEY = "token";

private:
    std::string m_login;
    std::string m_hashPassword;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_57C6E732_5A33_4CE6_8DAF_65613C3B4811