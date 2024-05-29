#ifndef H_13E53226_5A3F_4316_8230_B208A347F17A
#define H_13E53226_5A3F_4316_8230_B208A347F17A

#include <string_view>

#include <db_connector/data/project.h>
#include <db_connector/data/requirement.h>
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

class GetRelatedRequirements : public BaseTask
{
public:
    GetRelatedRequirements(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "get_related_requirements";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view DEPENDENT_KEY = "requirement_id";
    static constexpr std::string_view DEPENDENCIES_KEY = "related";
    static constexpr std::string_view DEPENDENCYID_KEY = "requirement_id";
    static constexpr std::string_view DEPENDENCYTYPE_KEY = "requirement_id";

private:
    db::data::Requirement m_req;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_13E53226_5A3F_4316_8230_B208A347F17A