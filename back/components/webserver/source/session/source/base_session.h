#ifndef H_F12D5F9D_0A17_4BCF_BFEB_D7BE7FF394B1
#define H_F12D5F9D_0A17_4BCF_BFEB_D7BE7FF394B1

#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <document_manager/document_manager.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include <task_manager/task_manager.h>

#include <session/session.h>

namespace beast = boost::beast;
namespace http = boost::beast::http;

namespace ws
{
namespace session
{

constexpr auto READ_TIMEOUT = std::chrono::seconds(30);

class BaseSession : public srv::tracer::TracerProvider, public ISession, public std::enable_shared_from_this<BaseSession>
{
public:
    BaseSession(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<srv::IAuthorizer> authorizer,
        std::shared_ptr<srv::IDocumentManager> documentManager,
        std::shared_ptr<taskmgr::ITaskManager> taskManager);

protected:
    void HandleRequest();

    void HandleApi(userid_t initiativeUserId);
    void HandleFile();

    http::message_generator PrepareResponse(std::string body, http::status status);

    ufa::Result Authenticate(userid_t& initiativeUserId);

protected:
    virtual void SendResponse(http::message_generator&& message) = 0;

protected:
    http::request<http::string_body> m_request;
    beast::flat_buffer m_buffer;

private:
    std::shared_ptr<srv::IAuthorizer> m_authorizer;
    std::shared_ptr<srv::IDocumentManager> m_documentManager;

    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
};

}  // namespace session
}  // namespace ws

#endif  // H_F12D5F9D_0A17_4BCF_BFEB_D7BE7FF394B1
