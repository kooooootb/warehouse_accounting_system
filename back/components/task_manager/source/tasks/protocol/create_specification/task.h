#ifndef H_AC6B82CC_B46A_440D_BF52_B7CC969213B8
#define H_AC6B82CC_B46A_440D_BF52_B7CC969213B8

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

class CreateSpecification : public BaseTask
{
public:
    CreateSpecification(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "create_specification";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view NAME_KEY = "spec_name";
    static constexpr std::string_view PROJECTID_KEY = "project";
    static constexpr std::string_view ID_KEY = "specification_id";

private:
    db::data::Specification m_spec;
    db::data::Project m_proj;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_AC6B82CC_B46A_440D_BF52_B7CC969213B8
