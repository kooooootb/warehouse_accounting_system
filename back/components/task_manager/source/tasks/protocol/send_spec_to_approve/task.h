#ifndef H_A0F0687C_8ADC_42CB_A5A1_693AC5B4704F
#define H_A0F0687C_8ADC_42CB_A5A1_693AC5B4704F

#include <string_view>

#include <db_connector/data/approve.h>
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

class SendSpecToApprove : public BaseTask
{
public:
    SendSpecToApprove(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "send_spec_to_approve";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view ID_KEY = "specification_id";

private:
    db::data::Specification m_spec;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_A0F0687C_8ADC_42CB_A5A1_693AC5B4704F