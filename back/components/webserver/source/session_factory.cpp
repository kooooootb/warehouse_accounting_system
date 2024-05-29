#include <filesystem>
#include <memory>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <authorizer/authorizer.h>
#include <db_connector/data/user.h>
#include <instrumental/check.h>
#include <instrumental/types.h>
#include <task_manager/task_manager.h>
#include <tracer/tracer.h>
#include <tracer/tracer_provider.h>
#include <utilities/document_manager.h>
#include <webserver/server.h>

#include "server_certificate.h"
#include "session.h"
#include "session_factory.h"

namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;

namespace ws
{
namespace
{

constexpr std::string_view AUTHORIZATION_TARGET = "/authorization";
constexpr std::string_view API_TARGET = "/api";

std::string_view GetMimeType(const std::filesystem::path& extension)
{
    if (extension == ".htm")
        return "text/html";
    if (extension == ".html")
        return "text/html";
    if (extension == ".js")
        return "application/javascript";
    if (extension == ".css")
        return "text/css";
    if (extension == ".php")
        return "text/html";
    if (extension == ".txt")
        return "text/plain";
    if (extension == ".json")
        return "application/json";
    if (extension == ".xml")
        return "application/xml";
    if (extension == ".swf")
        return "application/x-shockwave-flash";
    if (extension == ".flv")
        return "video/x-flv";
    if (extension == ".png")
        return "image/png";
    if (extension == ".jpe")
        return "image/jpeg";
    if (extension == ".jpeg")
        return "image/jpeg";
    if (extension == ".jpg")
        return "image/jpeg";
    if (extension == ".gif")
        return "image/gif";
    if (extension == ".bmp")
        return "image/bmp";
    if (extension == ".ico")
        return "image/vnd.microsoft.icon";
    if (extension == ".tiff")
        return "image/tiff";
    if (extension == ".tif")
        return "image/tiff";
    if (extension == ".svg")
        return "image/svg+xml";
    if (extension == ".svgz")
        return "image/svg+xml";
    return "application/text";
}

class BaseSession : public srv::tracer::TracerProvider, public ISession, public std::enable_shared_from_this<BaseSession>
{
public:
    BaseSession(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<auth::IAuthorizer> authorizer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_taskManager(std::move(taskManager))
        , m_authorizer(std::move(authorizer))
        , m_documentManager(std::move(documentManager))
    {
    }

protected:
    void HandleRequest()
    {
        if (m_request.method() != http::verb::get && m_request.method() != http::verb::post)
        {
            TRACE_ERR << TRACE_HEADER << "Request has incompatible verb: " << http::to_string(m_request.method());
            return SendResponse(PrepareResponse("Invalid verb", http::status::bad_request));
        }

        const auto target = m_request.target();
        if (target.empty() || target[0] != '/')
        {
            TRACE_ERR << TRACE_HEADER << "Request has invalid target: \"" << target << "\"";
            return SendResponse(PrepareResponse("Invalid target", http::status::bad_request));
        }

        if (target.size() > API_TARGET.size() + 2 && target.starts_with(API_TARGET))
        {
            TRACE_DBG << TRACE_HEADER << "Retrieved api request: " << target;
            db::data::User initiativeUser;

            // check authentication only when /api and not when authorizing
            if (!(target.size() == API_TARGET.size() + AUTHORIZATION_TARGET.size() && target.starts_with(API_TARGET) &&
                    target.ends_with(AUTHORIZATION_TARGET)))
            {
                // in this branch authentication required
                const auto authenticationResult = Authenticate(initiativeUser);
                if (authenticationResult == ufa::Result::WRONG_FORMAT)
                {
                    return SendResponse(PrepareResponse("Invalid authentication header", http::status::bad_request));
                }
                else if (authenticationResult == ufa::Result::UNAUTHORIZED)
                {
                    return SendResponse(PrepareResponse("Unauthorized", http::status::unauthorized));
                }
                CHECK_SUCCESS(authenticationResult);
            }

            return HandleApi(std::move(initiativeUser));
        }
        else
        {
            TRACE_DBG << TRACE_HEADER << "Retrieved file request: " << target;
            return HandleFile();
        }
    }

    void HandleApi(db::data::User initiativeUser)
    {
        auto target = m_request.target();
        target.remove_prefix(API_TARGET.size() + 1);

        const auto result = m_taskManager->AddTask(std::move(initiativeUser),
            target,
            std::move(m_request.body()),
            [this, self = shared_from_this()](std::string&& message, ufa::Result result)
            {
                if (result != ufa::Result::SUCCESS)
                {
                    http::status status = http::status::internal_server_error;

                    switch (result)
                    {
                        case ufa::Result::WRONG_FORMAT:
                            status = http::status::bad_request;
                            break;
                        case ufa::Result::NOT_FOUND:
                            status = http::status::not_found;
                            break;
                        case ufa::Result::UNAUTHORIZED:
                            status = http::status::unauthorized;
                            break;
                        default:
                            status = http::status::internal_server_error;
                    }

                    TRACE_ERR << TRACE_HEADER << "Task execution failed with error: " << result << ", responding with "
                              << static_cast<unsigned>(status);
                    SendResponse(PrepareResponse(std::move(message), status));
                }
                else
                {
                    SendResponse(PrepareResponse(std::move(message), http::status::ok));
                }
            });

        if (result == ufa::Result::WRONG_FORMAT)
        {
            TRACE_ERR << TRACE_HEADER << "Retrieved body in wrong format";
            return SendResponse(PrepareResponse("Invalid body format", http::status::bad_request));
        }

        if (result == ufa::Result::NOT_FOUND)
        {
            TRACE_ERR << TRACE_HEADER << "Targett not found" << target;
            return SendResponse(PrepareResponse("Invalid target", http::status::not_found));
        }

        CHECK_SUCCESS(result, "Error while parsing body:" << string_converters::ToString(result));
    }

    void HandleFile()
    {
        const auto target = m_request.target();

        auto firstWordEnd = target.find_first_of("?#", 1);
        if (firstWordEnd == target.npos)
            firstWordEnd = target.size();

        std::filesystem::path file(target.substr(1, firstWordEnd - 1));
        const auto result = m_documentManager->RestoreDocument(file);

        if (result == ufa::Result::WRONG_FORMAT)
        {
            TRACE_ERR << TRACE_HEADER << "Requested invalid file: " << file.c_str() << "\"";
            return SendResponse(PrepareResponse("Invalid filename", http::status::bad_request));
        }

        if (result == ufa::Result::NOT_FOUND)
        {
            TRACE_ERR << TRACE_HEADER << "Can't find requested file: " << file.c_str() << "\"";
            return SendResponse(PrepareResponse("File not found", http::status::not_found));
        }

        beast::error_code ec;
        http::file_body::value_type body;

        body.open(file.c_str(), beast::file_mode::scan, ec);
        CHECK_TRUE(!ec, "Error opening file: " << ec << ", message: " << ec.message());

        // Cache the size since we need it after the move
        auto const size = body.size();

        // Respond to HEAD request
        if (m_request.method() == http::verb::head)
        {
            http::response<http::empty_body> res{http::status::ok, m_request.version()};
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, GetMimeType(file.extension()));
            res.content_length(size);
            res.keep_alive(m_request.keep_alive());
            SendResponse(std::move(res));
        }

        // Respond to GET request
        http::response<http::file_body> res{std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, m_request.version())};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, GetMimeType(file.extension()));
        res.content_length(size);
        res.keep_alive(m_request.keep_alive());
        SendResponse(std::move(res));
    }

    http::message_generator PrepareResponse(std::string body, http::status status)
    {
        // cut on error
        bool keepAlive = status == http::status::ok ? m_request.keep_alive() : false;
        http::response<http::string_body> res{status, m_request.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(keepAlive);
        res.body() = std::move(body);
        res.prepare_payload();
        return res;
    }

    virtual void SendResponse(http::message_generator&& message) = 0;

    ufa::Result Authenticate(db::data::User& userData)
    {
        const auto token = m_request[AUTHORIZATION_HEADER];
        if (token.empty())
        {
            return ufa::Result::WRONG_FORMAT;
        }

        return m_authorizer->ValidateToken(token, userData);
    }

protected:
    http::request<http::string_body> m_request;
    beast::flat_buffer m_buffer;

private:
    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
    std::shared_ptr<auth::IAuthorizer> m_authorizer;
    std::shared_ptr<docmgr::IDocumentManager> m_documentManager;
};

class PlainSession : public BaseSession
{
public:
    PlainSession(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<auth::IAuthorizer> authorizer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager,
        tcp::socket&& socket)
        : BaseSession(std::move(tracer), std::move(taskManager), std::move(authorizer), std::move(documentManager))
        , m_stream(std::move(socket))
    {
    }

    void Run() override
    {
        asio::dispatch(m_stream.get_executor(),
            beast::bind_front_handler(&PlainSession::DoRead, std::static_pointer_cast<PlainSession>(shared_from_this())));
    }

private:
    void DoRead()
    {
        TRACE_DBG << TRACE_HEADER << "Reading plain";

        m_request = {};

        // Set the timeout.
        beast::get_lowest_layer(m_stream).expires_after(std::chrono::seconds(30));

        // Read a request
        http::async_read(m_stream,
            m_buffer,
            m_request,
            beast::bind_front_handler(&PlainSession::OnRead, std::static_pointer_cast<PlainSession>(shared_from_this())));
    }

    void OnRead(boost::system::error_code ec, std::size_t bytesTransferred)
    {
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

    void SendResponse(http::message_generator&& message) override
    {
        bool keepAlive = message.keep_alive();

        // Write the response
        beast::async_write(m_stream,
            std::move(message),
            beast::bind_front_handler(&PlainSession::OnWrite, std::static_pointer_cast<PlainSession>(shared_from_this()), keepAlive));
    }

    void OnWrite(bool keepAlive, boost::system::error_code ec, std::size_t bytesTransferred)
    {
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

    void DoClose()
    {
        beast::error_code ec;
        m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
        CHECK_TRUE(!ec, "Closing failed with error: " << ec << ", message: " << ec.message());
    }

private:
    beast::tcp_stream m_stream;
};

class SecuredSession : public BaseSession
{
public:
    SecuredSession(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<auth::IAuthorizer> authorizer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager,
        tcp::socket&& socket,
        ssl::context& sslContext)
        : BaseSession(std::move(tracer), std::move(taskManager), std::move(authorizer), std::move(documentManager))
        , m_sslStream(std::move(socket), sslContext)
    {
    }

    void Run() override
    {
        asio::dispatch(m_sslStream.get_executor(),
            beast::bind_front_handler(&SecuredSession::DoHandshake, std::static_pointer_cast<SecuredSession>(shared_from_this())));
    }

private:
    void DoHandshake()
    {
        TRACE_DBG << TRACE_HEADER << "Handshaking";

        // Set the timeout.
        beast::get_lowest_layer(m_sslStream).expires_after(std::chrono::seconds(30));

        // Perform the SSL handshake
        m_sslStream.async_handshake(ssl::stream_base::server,
            beast::bind_front_handler(&SecuredSession::OnHandshake, std::static_pointer_cast<SecuredSession>(shared_from_this())));
    }

    void OnHandshake(beast::error_code ec)
    {
        CHECK_TRUE(!ec, "Handshake failed with error: " << ec << ", message: " << ec.message());

        DoRead();
    }

    void DoRead()
    {
        TRACE_DBG << TRACE_HEADER << "Reading secured";

        m_request = {};

        // Set the timeout.
        beast::get_lowest_layer(m_sslStream).expires_after(std::chrono::seconds(30));

        // Read a request
        http::async_read(m_sslStream,
            m_buffer,
            m_request,
            beast::bind_front_handler(&SecuredSession::OnRead, std::static_pointer_cast<SecuredSession>(shared_from_this())));
    }

    void OnRead(boost::system::error_code ec, std::size_t bytesTransferred)
    {
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

    void SendResponse(http::message_generator&& message) override
    {
        bool keepAlive = message.keep_alive();

        // Write the response
        beast::async_write(m_sslStream,
            std::move(message),
            beast::bind_front_handler(&SecuredSession::OnWrite,
                std::static_pointer_cast<SecuredSession>(shared_from_this()),
                keepAlive));
    }

    void OnWrite(bool keepAlive, boost::system::error_code ec, std::size_t bytesTransferred)
    {
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

    void DoClose()
    {
        // Set the timeout.
        beast::get_lowest_layer(m_sslStream).expires_after(std::chrono::seconds(30));

        // Perform the SSL shutdown
        m_sslStream.async_shutdown(
            beast::bind_front_handler(&SecuredSession::OnClose, std::static_pointer_cast<SecuredSession>(shared_from_this())));
    }

    void OnClose(boost::system::error_code ec)
    {
        CHECK_TRUE(!ec, "Shutdown failed with error: " << ec << ", message: " << ec.message());
    }

private:
    beast::ssl_stream<beast::tcp_stream> m_sslStream;
};

class PlainSessionFactory : public srv::tracer::TracerProvider, public ISessionFactory
{
public:
    PlainSessionFactory(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<auth::IAuthorizer> authorizer)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_documentManager(std::move(documentManager))
        , m_taskManager(std::move(taskManager))
        , m_authorizer(std::move(authorizer))
    {
    }

    std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) override
    {
        return std::make_shared<PlainSession>(GetTracer(), m_taskManager, m_authorizer, m_documentManager, std::move(socket));
    }

private:
    std::shared_ptr<docmgr::IDocumentManager> m_documentManager;
    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
    std::shared_ptr<auth::IAuthorizer> m_authorizer;
};

class SecuredSessionFactory : public srv::tracer::TracerProvider, public ISessionFactory
{
public:
    SecuredSessionFactory(std::shared_ptr<srv::ITracer> tracer,
        std::shared_ptr<docmgr::IDocumentManager> documentManager,
        std::shared_ptr<taskmgr::ITaskManager> taskManager,
        std::shared_ptr<auth::IAuthorizer> authorizer)
        : srv::tracer::TracerProvider(std::move(tracer))
        , m_documentManager(std::move(documentManager))
        , m_taskManager(std::move(taskManager))
        , m_authorizer(std::move(authorizer))
        , m_sslContext(ssl::context::tlsv12)
    {
        LoadServerCertificate(m_sslContext);
    }

    std::shared_ptr<ISession> CreateSession(tcp::socket&& socket) override
    {
        return std::make_shared<SecuredSession>(GetTracer(),
            m_taskManager,
            m_authorizer,
            m_documentManager,
            std::move(socket),
            m_sslContext);
    }

private:
    std::shared_ptr<docmgr::IDocumentManager> m_documentManager;
    std::shared_ptr<taskmgr::ITaskManager> m_taskManager;
    std::shared_ptr<auth::IAuthorizer> m_authorizer;
    ssl::context m_sslContext;
};

}  // namespace

std::unique_ptr<ISessionFactory> ISessionFactory::CreateSessionFactory(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<taskmgr::ITaskManager> taskManager,
    std::shared_ptr<auth::IAuthorizer> authorizer,
    std::shared_ptr<docmgr::IDocumentManager> documentManager,
    bool isSecured)
{
    if (isSecured)
        return std::make_unique<SecuredSessionFactory>(std::move(tracer),
            std::move(documentManager),
            std::move(taskManager),
            std::move(authorizer));
    else
        return std::make_unique<PlainSessionFactory>(std::move(tracer),
            std::move(documentManager),
            std::move(taskManager),
            std::move(authorizer));
}

}  // namespace ws