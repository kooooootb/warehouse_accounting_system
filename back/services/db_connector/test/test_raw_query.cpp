#include <gtest/gtest.h>

#include <tracer/tracer_mock.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>

using namespace srv::db;

class RawQuerySerializationFixture : public testing::Test
{
public:
    RawQuerySerializationFixture() : m_tracerMock(srv::mock::TracerMock::Create()) {}

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
};

TEST_F(RawQuerySerializationFixture, HP)
{
    using namespace std::literals;

    constexpr std::string_view queryString =
        "SELECT first_name, last_name, salary FROM employee e1 WHERE salary > (SELECT AVG(salary) FROM employee e2 WHERE e1.departmet_id = e2.department_id) AND last_name <> $1"sv;

    params_t params;
    params.Append("John");

    const auto query = QueryFactory::CreateRaw(m_tracerMock, std::string(queryString), params);

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);
    EXPECT_EQ(query->ExtractParams(), params);
}
