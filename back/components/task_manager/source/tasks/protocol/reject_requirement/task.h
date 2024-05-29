#ifndef H_7671322A_450F_4DAA_BCED_FB2557BC168D
#define H_7671322A_450F_4DAA_BCED_FB2557BC168D

#include <string_view>

#include <db_connector/data/approve.h>
#include <instrumental/common.h>
#include <locator/service_locator.h>
#include <tasks/base_task.h>

#include <task_manager/callback.h>
#include <tracer/tracer.h>

namespace taskmgr
{
namespace tasks
{

class RejectSpecification : public BaseTask
{
public:
    RejectSpecification(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "approve_requirement";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view ID_KEY = "requirement_id";
    static constexpr std::string_view COMMENT_KEY = "comment";

private:
    db::data::Reject m_approve;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_7671322A_450F_4DAA_BCED_FB2557BC168D