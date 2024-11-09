#ifndef H_040A1DB0_C24D_4B60_908D_E7111257FBF6
#define H_040A1DB0_C24D_4B60_908D_E7111257FBF6

#include <nlohmann/json.hpp>

#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <locator/service_locator.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include <task_manager/callback.h>
#include <task_manager/task_info.h>

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
    /**
     * @throw when body is in wrong format
     */
    BaseTask(std::shared_ptr<srv::ITracer> tracer, const TaskInfo& taskInfo)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_initiativeUserId(taskInfo.initiativeUserid)
    {
        TRACE_INF << TRACE_HEADER << "Task identificator: " << taskInfo.identificator
                  << ", initiative user: " << taskInfo.initiativeUserid;
    }

    /**
     * @brief includes parse task from json and setting callback
     * was created to call overriden methods
     * @return ufa::Result SUCCESS on success, WRONG_FORMAT when json is invalid
     */
    ufa::Result Initialize(TaskInfo&& taskInfo)
    {
        m_callback = std::move(taskInfo.callback);
        return Parse(std::move(taskInfo.body));
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
    userid_t m_initiativeUserId;

private:
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
            TRACE_ERR << TRACE_HEADER << "invalid json, what(): " << ex.what();
            return ufa::Result::WRONG_FORMAT;
        }

        return ufa::Result::SUCCESS;
    }

private:
    callback_t m_callback;
};

}  // namespace tasks
}  // namespace taskmgr

#endif  // H_040A1DB0_C24D_4B60_908D_E7111257FBF6