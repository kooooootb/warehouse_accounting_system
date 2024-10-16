#ifndef H_46135285_358D_4E35_AF98_28BD10174C59
#define H_46135285_358D_4E35_AF98_28BD10174C59

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
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;

namespace ws
{
namespace session
{

class SecuredSession : public BaseSession
{
public:
    SecuredSession(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<srv::IAuthorizer> authorizer,
        std::shared_ptr<srv::IDocumentManager> documentManager,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        tcp::socket&& socket,
        ssl::context& sslContext);

    void Run() override;

protected:
    void SendResponse(http::message_generator&& message) override;

private:
    void DoHandshake();
    void OnHandshake(beast::error_code ec);

    void DoRead();
    void OnRead(boost::system::error_code ec, std::size_t bytesTransferred);

    void OnWrite(bool keepAlive, boost::system::error_code ec, std::size_t bytesTransferred);

    void DoClose();
    void OnClose(boost::system::error_code ec);

private:
    beast::ssl_stream<beast::tcp_stream> m_sslStream;
};

}  // namespace session
}  // namespace ws

#endif  // H_46135285_358D_4E35_AF98_28BD10174C59
