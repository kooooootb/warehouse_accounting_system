#include "plain_session.h"

namespace asio = boost::asio;

namespace ws
{
namespace session
{

PlainSession::PlainSession(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IAuthorizer> authorizer,
    std::shared_ptr<srv::IDocumentManager> documentManager,
    std::shared_ptr<taskmgr::ITaskManager> taskManager,
    tcp::socket&& socket)
    : BaseSession(std::move(tracer), std::move(authorizer), std::move(documentManager), std::move(taskManager))
    , m_stream(std::move(socket))
{
    TRACE_DBG << TRACE_HEADER;
}

void PlainSession::Run()
{
    TRACE_DBG << TRACE_HEADER;

    asio::dispatch(m_stream.get_executor(),
        beast::bind_front_handler(&PlainSession::DoRead, std::static_pointer_cast<PlainSession>(shared_from_this())));
}

void PlainSession::DoRead()
{
    TRACE_DBG << TRACE_HEADER;

    m_request = {};

    // Set the timeout.
    beast::get_lowest_layer(m_stream).expires_after(READ_TIMEOUT);

    // Read a request
    http::async_read(m_stream,
        m_buffer,
        m_request,
        beast::bind_front_handler(&PlainSession::OnRead, std::static_pointer_cast<PlainSession>(shared_from_this())));
}

void PlainSession::OnRead(boost::system::error_code ec, std::size_t bytesTransferred)
{
    TRACE_DBG << TRACE_HEADER;

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

void PlainSession::SendResponse(http::message_generator&& message)
{
    TRACE_DBG << TRACE_HEADER;

    bool keepAlive = message.keep_alive();

    // Write the response
    beast::async_write(m_stream,
        std::move(message),
        beast::bind_front_handler(&PlainSession::OnWrite, std::static_pointer_cast<PlainSession>(shared_from_this()), keepAlive));
}

void PlainSession::OnWrite(bool keepAlive, boost::system::error_code ec, std::size_t bytesTransferred)
{
    TRACE_DBG << TRACE_HEADER;

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

void PlainSession::DoClose()
{
    TRACE_DBG << TRACE_HEADER;

    beast::error_code ec;
    m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
    CHECK_TRUE(!ec, "Closing failed with error: " << ec << ", message: " << ec.message());
}

}  // namespace session
}  // namespace ws