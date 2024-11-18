#ifndef H_0437F7DB_BDFC_4A87_806F_B00CD88DFB74
#define H_0437F7DB_BDFC_4A87_806F_B00CD88DFB74

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

class GetUserList : public BaseTask
{
public:
    GetUserList(std::shared_ptr<srv::ITracer> tracer, std::shared_ptr<srv::IServiceLocator> locator, const TaskInfo& taskInfo);

    constexpr static TaskIdentificator GetIdentificator()
    {
        return TaskIdentificator::GetUserList;
    }

protected:
    ufa::Result ExecuteInternal(std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    ufa::Result ActualGetUserList(srv::IAccessor& accessor);

private:
    static constexpr std::string_view USER_ID_KEY = "user_id";
    static constexpr std::string_view LOGIN_KEY = "login";
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view CREATED_DATE_KEY = "created_date";
    static constexpr std::string_view CREATED_BY_KEY = "created_by";
    static constexpr std::string_view RESULT_KEY = "result";
    static constexpr std::string_view LIMIT_KEY = "limit";
    static constexpr std::string_view OFFSET_KEY = "offset";

private:
    std::vector<User> m_users;
    int64_t m_limit;
    int64_t m_offset;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_0437F7DB_BDFC_4A87_806F_B00CD88DFB74