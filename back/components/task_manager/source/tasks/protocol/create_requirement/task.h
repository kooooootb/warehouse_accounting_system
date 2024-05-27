#ifndef H_75E59473_AE65_4ED5_978B_AFA352268551
#define H_75E59473_AE65_4ED5_978B_AFA352268551

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
    CreateRequirement(std::shared_ptr<srv::ITracer> tracer, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "create_requirement";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view TYPE_KEY = "type_id";
    static constexpr std::string_view SPECIFICATION_ID_KEY = "in_specification";
    static constexpr std::string_view PROJECT_ID_KEY = "project_id";

private:
    std::string m_description;
    uint64_t m_type;
    uint64_t m_specificationId;
    uint64_t m_projectId;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_75E59473_AE65_4ED5_978B_AFA352268551