#include <db_connector/query/query.h>
#include <gtest/gtest.h>

#include <tracer/tracer_mock.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/delete_query_params.h>
#include <db_connector/query/query_factory.h>

using namespace srv::db;

class DeleteQuerySerializationFixture : public testing::Test
{
public:
    DeleteQuerySerializationFixture() : m_tracerMock(srv::mock::TracerMock::Create()) {}

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
};

TEST_F(DeleteQuerySerializationFixture, HP)
{
    auto options = std::make_unique<DeleteOptions>();
    DeleteValues values;

    options->table = Table::User;

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);

    options->condition = std::move(real);

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString = "DELETE FROM public.\"User\" WHERE (user_id = $1);";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);

    params_t expectedParams;
    expectedParams.Append(123);

    EXPECT_EQ(query->ExtractParams(), expectedParams);
}

TEST_F(DeleteQuerySerializationFixture, ThrowWithoutConditionInQueryOptions)
{
    auto options = std::make_unique<DeleteOptions>();

    options->table = Table::Color;

    EXPECT_ANY_THROW((static_cast<IQueryOptions&>(*options)).SerializeParametrized());
}

TEST_F(DeleteQuerySerializationFixture, ThrowWithoutConditionInQuery)
{
    auto options = std::make_unique<DeleteOptions>();
    DeleteValues values;

    options->table = Table::Color;

    EXPECT_ANY_THROW(QueryFactory::Create(m_tracerMock, std::move(options), std::move(values)));
}

TEST_F(DeleteQuerySerializationFixture, ThrowWithInvalidTableInQueryOptions)
{
    auto options = std::make_unique<DeleteOptions>();

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);
    options->condition = std::move(real);

    EXPECT_ANY_THROW((static_cast<IQueryOptions&>(*options)).SerializeParametrized());
}

TEST_F(DeleteQuerySerializationFixture, QueryIdentificator)
{
    auto options = std::make_unique<DeleteOptions>();
    DeleteValues values;

    options->table = Table::User;

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);

    options->condition = std::move(real);

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    EXPECT_EQ(query->GetIdentificator(), QueryIdentificator::DELETE);
    EXPECT_EQ(query->ExtractOptions()->GetIdentificator(), QueryIdentificator::DELETE);
}
