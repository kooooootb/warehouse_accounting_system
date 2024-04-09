#ifndef H_4656D1E6_D255_4E2A_9CBF_8F639D8D2DDA
#define H_4656D1E6_D255_4E2A_9CBF_8F639D8D2DDA

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

class GetProjectInfo : public BaseTask
{
public:
    GetProjectInfo(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "get_project_info";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view NAME_KEY = "name";
    static constexpr std::string_view PROJECTID_KEY = "project_id";
    static constexpr std::string_view REQUIREMENTIDS_KEY = "requirement_ids";
    static constexpr std::string_view SPECIFICATIONIDS_KEY = "specification_ids";

private:
    db::data::Project m_proj;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_4656D1E6_D255_4E2A_9CBF_8F639D8D2DDA
