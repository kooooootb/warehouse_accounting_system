#ifndef H_75E59473_AE65_4ED5_978B_AFA352268551
#define H_75E59473_AE65_4ED5_978B_AFA352268551

#include <string_view>

#include <db_connector/data/dependency.h>
#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tasks/base_task.h>

#include <task_manager/callback.h>
#include <tracer/tracer.h>

namespace taskmgr
{
namespace tasks
{

class CreateDependency : public BaseTask
{
public:
    CreateDependency(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "create_dependency";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view REQUIREMENT_ID_KEY = "requirement_id";
    static constexpr std::string_view DEPENDENCIES_IDS_KEY = "dependencies_ids";
    static constexpr std::string_view DEPENDENCY_ID_KEY = "requirement_id";
    static constexpr std::string_view DEPENDENCY_TYPE_KEY = "dependency_type";

private:
    std::vector<db::data::Dependency> m_deps;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_75E59473_AE65_4ED5_978B_AFA352268551