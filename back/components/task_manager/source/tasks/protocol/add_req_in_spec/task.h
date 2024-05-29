#ifndef H_0C487493_8D14_434F_97BA_724917262ADC
#define H_0C487493_8D14_434F_97BA_724917262ADC

#include <string_view>

#include <db_connector/data/requirement.h>
#include <db_connector/data/section.h>
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

class AddReqInSpec : public BaseTask
{
public:
    AddReqInSpec(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "add_req_in_spec";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view REQUIREMENT_ID_KEY = "requirement_id";
    static constexpr std::string_view SPECIFICATION_ID_KEY = "specification_id";
    static constexpr std::string_view SECTION_KEY = "section";
    static constexpr std::string_view SECTION_ID_KEY = "id";
    static constexpr std::string_view SECTION_NAME_KEY = "name";
    static constexpr std::string_view SECTION_REQNUM_KEY = "req_num_in_sec";
    static constexpr std::string_view SECTION_NUM_KEY = "sec_num_in_spec";

private:
    db::data::Requirement m_requirement;
    db::data::Specification m_specification;
    db::data::Section m_section;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_0C487493_8D14_434F_97BA_724917262ADC