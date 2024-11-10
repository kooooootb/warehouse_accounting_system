#include <future>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <hash/hash.h>
#include <instrumental/types.h>
#include <instrumental/user.h>
#include <locator/service_locator.h>
#include <task_manager/task_identificator.h>
#include <task_manager/task_info.h>
#include <task_manager/task_manager.h>

#include <authorizer/authorizer_mock.h>
#include <locator/service_locator_mock.h>
#include <tracer/tracer_mock.h>

#include "../source/task_manager.h"

#include "gmock/gmock.h"

using namespace taskmgr;
using namespace taskmgr::tasks;
using namespace testing;

class AuthorizationFixture : public testing::Test
{
protected:
    AuthorizationFixture() : m_tracerMock(srv::mock::TracerMock::Create()), m_locatorMock(srv::mock::ServiceLocatorMock::Create())
    {
        m_locatorMock->RegisterMock(m_tracerMock);
    }

    std::shared_ptr<ITaskManager> CreateTaskManager()
    {
        return ITaskManager::Create(m_locatorMock);
    }

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
    std::shared_ptr<srv::mock::ServiceLocatorMock> m_locatorMock;
};

TEST_F(AuthorizationFixture, HP)
{
    using namespace std::literals;

    TaskInfo taskInfo;
    taskInfo.identificator = TaskIdentificator::Authorization;
    taskInfo.initiativeUserid = 100;

    taskInfo.body = R"({
        "login": "superuser",
        "password": "admin"
    })";

    std::promise<ufa::Result> promise;
    std::future<ufa::Result> future = promise.get_future();
    json outbody;

    taskInfo.callback = [&promise, &outbody](std::string&& body, ufa::Result result)
    {
        outbody = json::parse(std::move(body));
        promise.set_value(result);
    };

    auto expectedUserid = 123;
    auto expectedToken = "supertoken"sv;

    auto authorizerMock = srv::mock::AuthorizerMock::Create();
    EXPECT_CALL(*authorizerMock, GenerateToken(Eq("superuser"sv), Eq(util::hash::HashToBase64("admin"sv)), _, _))
        .WillRepeatedly(DoAll(SetArgReferee<2>(expectedToken), SetArgReferee<3>(expectedUserid), Return(ufa::Result::SUCCESS)));

    m_locatorMock->RegisterMock(authorizerMock);

    auto tm = CreateTaskManager();

    tm->AddTask(std::move(taskInfo));

    EXPECT_EQ(future.get(), ufa::Result::SUCCESS);
    EXPECT_EQ(outbody["user_id"].get<int64_t>(), expectedUserid);
    EXPECT_EQ(outbody["token"].get<std::string>(), expectedToken);
}

TEST_F(AuthorizationFixture, WrongCredentials)
{
    using namespace std::literals;

    TaskInfo taskInfo;
    taskInfo.identificator = TaskIdentificator::Authorization;
    taskInfo.initiativeUserid = 100;

    taskInfo.body = R"({
        "login": "superuser",
        "password": "admin"
    })";

    std::promise<ufa::Result> promise;
    std::future<ufa::Result> future = promise.get_future();
    json outbody;

    taskInfo.callback = [&promise, &outbody](std::string&& body, ufa::Result result)
    {
        outbody = json::parse(std::move(body));
        promise.set_value(result);
    };

    auto authorizerMock = srv::mock::AuthorizerMock::Create();
    EXPECT_CALL(*authorizerMock, GenerateToken(_, _, _, _)).WillRepeatedly(Return(ufa::Result::UNAUTHORIZED));

    m_locatorMock->RegisterMock(authorizerMock);

    auto tm = CreateTaskManager();

    tm->AddTask(std::move(taskInfo));

    EXPECT_EQ(future.get(), ufa::Result::UNAUTHORIZED);
    EXPECT_FALSE(outbody.contains("user_id"));
    EXPECT_FALSE(outbody.contains("token"));
}
