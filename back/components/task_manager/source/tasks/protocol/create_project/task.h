#ifndef H_4C801F1A_92EB_4320_A914_DFCBFA8C01C1
#define H_4C801F1A_92EB_4320_A914_DFCBFA8C01C1

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

class CreateProject : public BaseTask
{
public:
    CreateProject(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "create_project";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view NAME_KEY = "project_name";
    static constexpr std::string_view ID_KEY = "project_id";

private:
    db::data::Project m_proj;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_4C801F1A_92EB_4320_A914_DFCBFA8C01C1