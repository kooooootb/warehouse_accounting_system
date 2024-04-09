#ifndef H_6D8ED463_E2C1_43AD_908D_582BA347970B
#define H_6D8ED463_E2C1_43AD_908D_582BA347970B

#include <string_view>

#include <db_connector/data/project.h>
#include <db_connector/data/specification.h>
#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tasks/base_task.h>

#include <task_manager/callback.h>
#include <tracer/tracer.h>

namespace taskmgr
{
namespace tasks
{

class GetProjects : public BaseTask
{
public:
    GetProjects(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "get_projects";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view PROJECTIDS_KEY = "project_ids";
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_6D8ED463_E2C1_43AD_908D_582BA347970B
