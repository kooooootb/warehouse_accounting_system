#include <boost/beast/version.hpp>
#include <exception>

#include <authorizer/authorizer.h>
#include <instrumental/types.h>
#include <task_manager/callback.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>

#include "base_session.h"

namespace ws
{
namespace session
{

namespace
{

constexpr std::string_view AUTHORIZATION_TARGET = "/authorization";
constexpr std::string_view AUTHENTICATE_HEADER = "Authorization";
constexpr std::string_view API_TARGET = "/api";

constexpr http::verb ALLOWED_VERBS[] = {http::verb::get, http::verb::post};

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

http::status ConvertToHttpStatus(ufa::Result result)
{
    switch (result)
    {
        case ufa::Result::WRONG_FORMAT:
            return http::status::bad_request;
        case ufa::Result::NOT_FOUND:
            return http::status::not_found;
        case ufa::Result::UNAUTHORIZED:
            return http::status::unauthorized;
        case ufa::Result::VIOLATION:
            return http::status::unprocessable_entity;
        default:
            // other errors related to internal errors like ERROR etc
            return http::status::internal_server_error;
    }
}

taskmgr::TaskIdentificator ParseTaskIdentificator(std::string_view target, http::verb verb)
{
    using TI = taskmgr::TaskIdentificator;

    if (target == "authorization")
        return TI::Authorization;
    if (target == "users/create")
        return TI::CreateUser;
    if (target == "products/create")
        return TI::ProductsCreate;
    if (target == "products/move")
        return TI::ProductsMove;
    if (target == "products/remove")
        return TI::ProductsRemove;
    if (target == "reports/byperiod")
        return TI::ReportsByPeriod;
    if (target == "warehouses/create")
        return TI::CreateWarehouse;

    CHECK_SUCCESS(ufa::Result::NOT_FOUND,
        "Couldn't parse task identificator. target: " << target << ", verb: " << http::to_string(verb));
}

}  // namespace

BaseSession::BaseSession(std::shared_ptr<srv::ITracer> tracer,
    std::shared_ptr<srv::IAuthorizer> authorizer,
    std::shared_ptr<srv::IDocumentManager> documentManager,
    std::shared_ptr<taskmgr::ITaskManager> taskManager)
    : srv::tracer::TracerProvider(std::move(tracer))
    , m_authorizer(std::move(authorizer))
    , m_documentManager(std::move(documentManager))
    , m_taskManager(std::move(taskManager))
{
    TRACE_INF << TRACE_HEADER;
}

void BaseSession::HandleRequest()
{
    TRACE_INF << TRACE_HEADER;

    if (!std::any_of(std::cbegin(ALLOWED_VERBS),
            std::cend(ALLOWED_VERBS),
            [method = this->m_request.method()](http::verb allowedMethod) -> bool
            {
                return allowedMethod == method;
            }))
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
        TRACE_INF << TRACE_HEADER << "Retrieved api request: " << target;
        userid_t initiativeUserId = 0;

        // check authentication only when /api and not when authorizing
        if (!(target.size() == API_TARGET.size() + AUTHORIZATION_TARGET.size() && target.starts_with(API_TARGET) &&
                target.ends_with(AUTHORIZATION_TARGET)))
        {
            // in this branch authentication required
            const auto authenticationResult = Authenticate(initiativeUserId);

            if (authenticationResult == ufa::Result::WRONG_FORMAT)
            {
                return SendResponse(PrepareResponse("Invalid authentication header", http::status::unauthorized));
            }
            else if (authenticationResult == ufa::Result::UNAUTHORIZED)
            {
                return SendResponse(PrepareResponse("Unauthorized", http::status::unauthorized));
            }
            CHECK_SUCCESS(authenticationResult);
        }

        return HandleApi(std::move(initiativeUserId));
    }
    else
    {
        TRACE_INF << TRACE_HEADER << "Retrieved file request: " << target;
        return HandleFile();
    }
}

void BaseSession::HandleApi(userid_t initiativeUserId)
{
    TRACE_INF << TRACE_HEADER;

    auto target = m_request.target();
    target.remove_prefix(API_TARGET.size() + 1);

    taskmgr::TaskInfo taskInfo;

    try
    {
        taskInfo.identificator = ParseTaskIdentificator(target, m_request.method());
    }
    catch (const std::exception& ex)
    {
        TRACE_ERR << TRACE_HEADER << ex.what();
        return SendResponse(PrepareResponse("Task not found", http::status::not_found));
    }

    taskInfo.callback = [this, self = shared_from_this()](std::string&& message, ufa::Result result)
    {
        if (result != ufa::Result::SUCCESS)
        {
            const auto status = ConvertToHttpStatus(result);

            TRACE_ERR << TRACE_HEADER << "Task execution failed with error: " << result << ", responding with "
                      << static_cast<unsigned>(status) << ", message: " << message;
            SendResponse(PrepareResponse(std::move(message), status));
        }
        else
        {
            TRACE_DBG << TRACE_HEADER << "Responding ok with message: " << message;
            SendResponse(PrepareResponse(std::move(message), http::status::ok));
        }
    };

    taskInfo.body = std::move(m_request.body());
    taskInfo.initiativeUserid = initiativeUserId;

    const auto result = m_taskManager->AddTask(std::move(taskInfo));

    if (result == ufa::Result::WRONG_FORMAT)
    {
        TRACE_ERR << TRACE_HEADER << "Retrieved body in wrong format";
        return SendResponse(PrepareResponse("Invalid body format", http::status::bad_request));
    }

    if (result == ufa::Result::NOT_FOUND)
    {
        TRACE_ERR << TRACE_HEADER << "Task not implemented, target: " << target;
        return SendResponse(PrepareResponse("Task not found", http::status::not_found));
    }

    CHECK_SUCCESS(result, "Error while parsing body: " << string_converters::ToString(result));
}

void BaseSession::HandleFile()
{
    TRACE_INF << TRACE_HEADER;

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

http::message_generator BaseSession::PrepareResponse(std::string body, http::status status)
{
    TRACE_INF << TRACE_HEADER;

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

ufa::Result BaseSession::Authenticate(userid_t& userId)
{
    TRACE_INF << TRACE_HEADER;

    const auto token = m_request[AUTHENTICATE_HEADER];
    if (token.empty())
    {
        return ufa::Result::WRONG_FORMAT;
    }

    return m_authorizer->ValidateToken(token, userId);
}

}  // namespace session
}  // namespace ws
