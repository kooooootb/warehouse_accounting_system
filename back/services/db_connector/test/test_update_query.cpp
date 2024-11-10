#include <db_connector/query/query_options.h>
#include <gtest/gtest.h>

#include <tracer/tracer_mock.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/update_query_params.h>

using namespace srv::db;

class UpdateQuerySerializationFixture : public testing::Test
{
public:
    UpdateQuerySerializationFixture() : m_tracerMock(srv::mock::TracerMock::Create()) {}

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
};

TEST_F(UpdateQuerySerializationFixture, HP)
{
    auto options = std::make_unique<UpdateOptions>();
    UpdateValues values;

    options->table = Table::User;
    options->columns = {Column::name, Column::password_hashed};

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);

    options->condition = std::move(real);

    values.values.Append("Jack").Append("admin2");

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString = R"(UPDATE public."User" SET (name = $1, password_hashed = $2) WHERE (user_id = $3);)";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);

    params_t expectedParams;
    expectedParams.Append("Jack").Append("admin2").Append(123);

    EXPECT_EQ(query->ExtractParams(), expectedParams);
}

TEST_F(UpdateQuerySerializationFixture, ThrowWithoutConditionInQueryOptions)
{
    auto options = std::make_unique<UpdateOptions>();

    options->table = Table::Color;
    options->columns = {Column::color_value};

    EXPECT_ANY_THROW((static_cast<IQueryOptions&>(*options)).SerializeParametrized());
}

TEST_F(UpdateQuerySerializationFixture, ThrowWithoutColumnsInQueryOptions)
{
    auto options = std::make_unique<UpdateOptions>();

    options->table = Table::Color;

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);
    options->condition = std::move(real);

    EXPECT_ANY_THROW((static_cast<IQueryOptions&>(*options)).SerializeParametrized());
}

TEST_F(UpdateQuerySerializationFixture, ThrowWithInvalidTableInQueryOptions)
{
    auto options = std::make_unique<UpdateOptions>();

    options->columns = {Column::color_value};

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);
    options->condition = std::move(real);

    EXPECT_ANY_THROW((static_cast<IQueryOptions&>(*options)).SerializeParametrized());
}

TEST_F(UpdateQuerySerializationFixture, ThrowWithoutConditionInQuery)
{
    auto options = std::make_unique<UpdateOptions>();
    UpdateValues values;

    options->table = Table::Color;
    options->columns = {Column::color_value};

    values.values.Append("Jack").Append("admin2");

    EXPECT_ANY_THROW(QueryFactory::Create(m_tracerMock, std::move(options), std::move(values)));
}

TEST_F(UpdateQuerySerializationFixture, ThrowWithUnmatchingColumnsInQuery)
{
    auto options = std::make_unique<UpdateOptions>();
    UpdateValues values;

    options->table = Table::Color;
    options->columns = {Column::color_value};

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);
    options->condition = std::move(real);

    values.values.Append("Jack").Append("admin2").Append("odd");

    EXPECT_ANY_THROW(QueryFactory::Create(m_tracerMock, std::move(options), std::move(values)));
}

TEST_F(UpdateQuerySerializationFixture, QueryIdentificator)
{
    auto options = std::make_unique<UpdateOptions>();
    UpdateValues values;

    options->table = Table::User;
    options->columns = {Column::name, Column::password_hashed};

    auto real = CreateRealCondition(Column::user_id, 123, RealConditionType::Equal);

    options->condition = std::move(real);

    values.values.Append("Jack").Append("admin2");

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    EXPECT_EQ(query->GetIdentificator(), QueryIdentificator::UPDATE);
    EXPECT_EQ(query->ExtractOptions()->GetIdentificator(), QueryIdentificator::UPDATE);
}
