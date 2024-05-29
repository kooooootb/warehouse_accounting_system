#ifndef H_D35C2F16_1F4C_4016_9FE1_69CD7163ECEA
#define H_D35C2F16_1F4C_4016_9FE1_69CD7163ECEA

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

class CreateRequirement : public BaseTask
{
public:
    CreateRequirement(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "create_requirement";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view TYPE_KEY = "type";
    static constexpr std::string_view PROJECT_ID_KEY = "project_id";
    static constexpr std::string_view ID_KEY = "requirement_id";

private:
    db::data::Requirement m_req;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_D35C2F16_1F4C_4016_9FE1_69CD7163ECEA