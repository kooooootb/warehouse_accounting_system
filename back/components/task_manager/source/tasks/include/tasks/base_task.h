#ifndef H_040A1DB0_C24D_4B60_908D_E7111257FBF6
#define H_040A1DB0_C24D_4B60_908D_E7111257FBF6

#include <nlohmann/json.hpp>

#include <dependency_manager/dependency_manager.h>
#include <instrumental/types.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include <task_manager/callback.h>

using json = nlohmann::json;

namespace taskmgr
{
namespace tasks
{

/**
 * @brief task will store values from string for later execution
 * each implementation will provide HandleTask specialization for executing
 */
class BaseTask : public srv::tracer::TracerProvider
{
public:
    BaseTask(std::shared_ptr<srv::ITracer> tracer, db::data::User user, Callback&& callback)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_callback(std::move(callback))
        , m_initiativeUser(std::move(user))
    {
    }

    /**
     * @brief parse task from json
     * @return ufa::Result SUCCESS on success, WRONG_FORMAT when json is invalid
     */
    ufa::Result Parse(std::string&& jsonStr)
    {
        TRACE_DBG << TRACE_HEADER << "Parsing task: " << jsonStr;  // will leak sensitive

        try
        {
            auto json = json::parse(std::move(jsonStr));
            ParseInternal(std::move(json));
        }
        catch (const std::exception& ex)
        {
            TRACE_ERR << TRACE_HEADER << "invalid json:";
            return ufa::Result::WRONG_FORMAT;
        }

        return ufa::Result::SUCCESS;
    }

    void Execute(const deps::IDependencyManager& depManager)
    {
        std::string response;
        const auto result = ExecuteInternal(depManager, response);

        TRACE_DBG << TRACE_HEADER << "Calling callback with response: " << response;
        m_callback(std::move(response), result);
    }

protected:
    virtual ufa::Result ExecuteInternal(const deps::IDependencyManager& depManager, std::string& result) = 0;
    virtual void ParseInternal(json&& json) = 0;

protected:
    db::data::User m_initiativeUser;

private:
    Callback m_callback;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_040A1DB0_C24D_4B60_908D_E7111257FBF6