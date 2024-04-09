#ifndef H_514CD3E2_D326_4C83_8CD9_8C7C2161BB99
#define H_514CD3E2_D326_4C83_8CD9_8C7C2161BB99

#include <fstream>
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

class GenerateReport : public BaseTask
{
public:
    GenerateReport(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback);

    constexpr static std::string_view GetTarget()
    {
        return "generate_report";
    }

protected:
    ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) override;
    void ParseInternal(json&& json) override;

private:
    static constexpr std::string_view ID_KEY = "specification_id";
    static constexpr std::string_view URL_KEY = "url";

private:
    void AppendToFile(std::ofstream& fs, const db::data::Requirement& req);

private:
    db::data::Specification m_spec;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_514CD3E2_D326_4C83_8CD9_8C7C2161BB99
