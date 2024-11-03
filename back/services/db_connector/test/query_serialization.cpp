#include <gtest/gtest.h>

#include <tracer/mock/tracer_mock.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>

using namespace srv::db;

class QuerySerializationFixture : public testing::Test
{
public:
    QuerySerializationFixture() : m_tracerMock(srv::mock::TracerMock::Create()) {}

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
};

TEST_F(QuerySerializationFixture, Select)
{
    auto options = std::make_unique<SelectOptions>();
    auto values = std::make_unique<SelectValues>();

    options->table = Table::User;
    options->columns = {Column::user_id, Column::name, Column::created_by};

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString = "SELECT user_id, name, created_by FROM User;";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);
}

TEST_F(QuerySerializationFixture, Select_WithJoin)
{
    auto options = std::make_unique<SelectOptions>();
    auto values = std::make_unique<SelectValues>();

    options->table = Table::User;
    options->columns = {Column::user_id, Column::name, Column::main_color};

    Join join;
    join.leftColumn = Column::user_id;
    join.joiningColumn = Column::created_by;
    join.joiningTable = Table::Product;
    join.type = JoinType::INNER;

    options->joins.emplace_back(std::move(join));

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString = "SELECT user_id, name, main_color FROM User INNER JOIN Product ON User.user_id=Product.created_by;";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);
}
