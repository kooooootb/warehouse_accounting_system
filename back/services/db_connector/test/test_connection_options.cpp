#include <gtest/gtest.h>

#include <tracer/mock/tracer_mock.h>

#include <db_connector/settings.h>

#include "../source/connection/source/connection_options.h"
#include "gmock/gmock.h"

using namespace srv::db;
using namespace srv::db::conn;
using namespace testing;

class ConnectionOptionsFixture : public testing::Test
{
public:
    ConnectionOptionsFixture() : m_tracerMock(srv::mock::TracerMock::Create()) {}

    std::vector<std::string> Split(std::string_view input, std::string_view separator = " ")
    {
        std::vector<std::string> result;

        auto sepIndex = input.find(separator);
        while (sepIndex != input.npos)
        {
            result.emplace_back(input.substr(0, sepIndex));
            input = input.substr(sepIndex + separator.size());
            sepIndex = input.find(separator);
        }

        return result;
    }

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
};

TEST_F(ConnectionOptionsFixture, HP)
{
    DBConnectorSettings settings;

    settings.dbmsAddress = "12.12.12.12";
    settings.dbmsPort = 3451u;
    settings.dbmsDbname = "WAS";
    settings.dbmsUser = "user1";
    settings.dbmsPassword = "password1";

    ConnectionOptions options(m_tracerMock);
    options.SetSettings(settings);

    std::vector<std::string> expected = {"host=12.12.12.12", "port=3451", "dbname=WAS", "user=user1", "password=password1"};
    auto result = Split(options.GetConnectionString());

    EXPECT_THAT(result, IsSubsetOf(expected));
}

TEST_F(ConnectionOptionsFixture, SetSettings)
{
    DBConnectorSettings settings;

    settings.dbmsAddress = "12.12.12.12";
    settings.dbmsPort = 3451u;
    settings.dbmsDbname = "WAS";
    settings.dbmsUser = "user1";
    settings.dbmsPassword = "password1";

    ConnectionOptions options(m_tracerMock);
    options.SetSettings(settings);

    std::vector<std::string> expected = {"host=12.12.12.12", "port=3451", "dbname=WAS", "user=user1", "password=password1"};
    auto result = Split(options.GetConnectionString());

    EXPECT_THAT(result, IsSubsetOf(expected));

    DBConnectorSettings newSettings;

    newSettings.dbmsDbname = "NEWWAS";

    options.SetSettings(newSettings);

    std::vector<std::string> newExpected = {"host=12.12.12.12", "port=3451", "dbname=NEWWAS", "user=user1", "password=password1"};
    auto newResult = Split(options.GetConnectionString());

    EXPECT_THAT(newResult, IsSubsetOf(newExpected));
}
