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

#include "secured_session.h"
#include "server_certificate.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;

namespace ws
{
namespace session
{

SecuredSession::SecuredSession(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IAuthorizer> authorizer,
    std::shared_ptr<srv::IDocumentManager> documentManager,
    std::shared_ptr<taskmgr::ITaskManager> taskManager,
    tcp::socket&& socket,
    ssl::context& sslContext)
    : BaseSession(std::move(tracer), std::move(authorizer), std::move(documentManager), std::move(taskManager))
    , m_sslStream(std::move(socket), sslContext)
{
    TRACE_INF << TRACE_HEADER;
}

void SecuredSession::Run()
{
    TRACE_INF << TRACE_HEADER;

    asio::dispatch(m_sslStream.get_executor(),
        beast::bind_front_handler(&SecuredSession::DoHandshake, std::static_pointer_cast<SecuredSession>(shared_from_this())));
}

void SecuredSession::SendResponse(http::message_generator&& message)
{
    TRACE_INF << TRACE_HEADER;

    bool keepAlive = message.keep_alive();

    // Write the response
    beast::async_write(m_sslStream,
        std::move(message),
        beast::bind_front_handler(&SecuredSession::OnWrite, std::static_pointer_cast<SecuredSession>(shared_from_this()), keepAlive));
}

void SecuredSession::DoHandshake()
{
    TRACE_INF << TRACE_HEADER;

    // Set the timeout.
    beast::get_lowest_layer(m_sslStream).expires_after(std::chrono::seconds(30));

    // Perform the SSL handshake
    m_sslStream.async_handshake(ssl::stream_base::server,
        beast::bind_front_handler(&SecuredSession::OnHandshake, std::static_pointer_cast<SecuredSession>(shared_from_this())));
}

void SecuredSession::OnHandshake(beast::error_code ec)
{
    TRACE_INF << TRACE_HEADER;

    CHECK_TRUE(!ec, "Handshake failed with error: " << ec << ", message: " << ec.message());

    DoRead();
}

void SecuredSession::DoRead()
{
    TRACE_INF << TRACE_HEADER;

    m_request = {};

    // Set the timeout.
    beast::get_lowest_layer(m_sslStream).expires_after(std::chrono::seconds(30));

    // Read a request
    http::async_read(m_sslStream,
        m_buffer,
        m_request,
        beast::bind_front_handler(&SecuredSession::OnRead, std::static_pointer_cast<SecuredSession>(shared_from_this())));
}

void SecuredSession::OnRead(boost::system::error_code ec, std::size_t bytesTransferred)
{
    TRACE_INF << TRACE_HEADER;

    boost::ignore_unused(bytesTransferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream)
        return DoClose();
    CHECK_TRUE(!ec, "Read failed with error: " << ec << ", message: " << ec.message());

    try
    {
        HandleRequest();
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << "Caught exception while handling request, target: " << m_request.target() << ", body: \""
                  << std::string(m_request.body()) << "\"";
        SendResponse(PrepareResponse("Server Error", http::status::internal_server_error));
        throw;
    }
}

void SecuredSession::OnWrite(bool keepAlive, boost::system::error_code ec, std::size_t bytesTransferred)
{
    TRACE_INF << TRACE_HEADER;

    boost::ignore_unused(bytesTransferred);

    CHECK_TRUE(!ec, "Write failed with error: " << ec << ", message: " << ec.message());

    if (!keepAlive)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        return DoClose();
    }

    // Read another request
    DoRead();
}

void SecuredSession::DoClose()
{
    TRACE_INF << TRACE_HEADER;

    // Set the timeout.
    beast::get_lowest_layer(m_sslStream).expires_after(std::chrono::seconds(30));

    // Perform the SSL shutdown
    m_sslStream.async_shutdown(
        beast::bind_front_handler(&SecuredSession::OnClose, std::static_pointer_cast<SecuredSession>(shared_from_this())));
}

void SecuredSession::OnClose(boost::system::error_code ec)
{
    TRACE_INF << TRACE_HEADER;

    CHECK_TRUE(!ec, "Shutdown failed with error: " << ec << ", message: " << ec.message());
}

}  // namespace session
}  // namespace ws
