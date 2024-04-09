#ifndef H_575285E3_73FE_4BEA_A4A0_6B3E1802948B
#define H_575285E3_73FE_4BEA_A4A0_6B3E1802948B

#include <string_view>

#include <db_connector/data/requirement.h>
#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tasks/base_task.h>

#include <task_manager/callback.h>
#include <tracer/tracer.h>

namespace taskmgr
{
namespace tasks
{

class EditRequirement : public BaseTask
{
public:
    EditRequirement(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "edit_requirement";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view IMPACTED_KEY = "impacted";
    static constexpr std::string_view DESCRIPTION_KEY = "description";
    static constexpr std::string_view ID_KEY = "requirement_id";

private:
    db::data::Requirement m_editReq;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_575285E3_73FE_4BEA_A4A0_6B3E1802948B
