#ifndef H_89F903D3_BDC4_47DE_8862_05ED513281AD
#define H_89F903D3_BDC4_47DE_8862_05ED513281AD

#include <string_view>

#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tasks/base_task.h>

#include <task_manager/callback.h>
#include <tracer/tracer.h>

namespace taskmgr
{
namespace tasks
{

class AddUser : public BaseTask
{
public:
    AddUser(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "add_user";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view FIRST_NAME_ID_KEY = "first_name";
    static constexpr std::string_view SECOND_NAME_ID_KEY = "last_name";
    static constexpr std::string_view LOGIN_ID_KEY = "login";
    static constexpr std::string_view PERPASSWORD_ID_KEY = "per_password";
    static constexpr std::string_view PERROLE_ID_KEY = "per_role";

private:
    db::data::User m_user;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_89F903D3_BDC4_47DE_8862_05ED513281AD
