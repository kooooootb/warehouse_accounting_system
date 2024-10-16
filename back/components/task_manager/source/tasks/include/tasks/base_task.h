#ifndef H_040A1DB0_C24D_4B60_908D_E7111257FBF6
#define H_040A1DB0_C24D_4B60_908D_E7111257FBF6

#include <nlohmann/json.hpp>

#include <authorizer/authorizer.h>
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
 */
class BaseTask : public srv::tracer::TracerProvider
{
public:
    BaseTask(std::shared_ptr<srv::ITracer> tracer, srv::auth::userid_t userId, Callback&& callback)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_initiativeUserId(std::move(userId))
        , m_callback(std::move(callback))
    {
        TRACE_INF << TRACE_HEADER;
    }

    /**
     * @brief parse task from json
     * @return ufa::Result SUCCESS on success, WRONG_FORMAT when json is invalid
     */
    ufa::Result Parse(std::string&& jsonStr)
    {
        TRACE_DBG << TRACE_HEADER << "Parsing task: " << jsonStr;  // will leak sensitive, todo

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

    void Execute(const srv::IServiceLocator& locator)
    {
        TRACE_INF << TRACE_HEADER;

        std::string response;
        const auto result = ExecuteInternal(locator, response);

        TRACE_DBG << TRACE_HEADER << "Calling callback with response: " << response;
        m_callback(std::move(response), result);
    }

protected:
    virtual ufa::Result ExecuteInternal(const srv::IServiceLocator& locator, std::string& result) = 0;
    virtual void ParseInternal(json&& json) = 0;

protected:
    srv::auth::userid_t m_initiativeUserId;

private:
    Callback m_callback;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_040A1DB0_C24D_4B60_908D_E7111257FBF6