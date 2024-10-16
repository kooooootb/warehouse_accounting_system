#ifndef H_F443B705_3DBF_4F1A_813C_E2C2D6F5F66A
#define H_F443B705_3DBF_4F1A_813C_E2C2D6F5F66A

#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <instrumental/common.h>

#include <authorizer/authorizer.h>
#include <document_manager/document_manager.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>

#include <task_manager/task_manager.h>

#include "base_session.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

namespace ws
{
namespace session
{

class PlainSession : public BaseSession
{
public:
    PlainSession(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<srv::IAuthorizer> authorizer,
        std::shared_ptr<srv::IDocumentManager> documentManager,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        tcp::socket&& socket);

    void Run() override;

protected:
    void SendResponse(http::message_generator&& message) override;

private:
    void DoRead();

    void OnRead(boost::system::error_code ec, std::size_t bytesTransferred);
    void OnWrite(bool keepAlive, boost::system::error_code ec, std::size_t bytesTransferred);

    void DoClose();

private:
    beast::tcp_stream m_stream;
};

}  // namespace session
}  // namespace ws

#endif  // H_F443B705_3DBF_4F1A_813C_E2C2D6F5F66A