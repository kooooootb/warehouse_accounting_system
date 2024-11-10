#include <gtest/gtest.h>

#include <tracer/tracer_mock.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/insert_query_params.h>
#include <db_connector/query/query_factory.h>

using namespace srv::db;

class InsertQuerySerializationFixture : public testing::Test
{
public:
    InsertQuerySerializationFixture() : m_tracerMock(srv::mock::TracerMock::Create()) {}

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
};

TEST_F(InsertQuerySerializationFixture, HP)
{
    auto options = std::make_unique<InsertOptions>();
    InsertValues values;

    options->table = Table::User;
    options->columns = {Column::login, Column::name, Column::password_hashed, Column::created_by};

    params_t values1;
    values1.Append("123").Append("John").Append("admin").Append(1);
    values.values.emplace_back(std::move(values1));

    params_t values2;
    values2.Append("456").Append("Jack").Append("admin2").Append(1);
    values.values.emplace_back(std::move(values2));

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString =
        R"(INSERT INTO public."User" (login, name, password_hashed, created_by) VALUES ($1, $2, $3, $4), ($5, $6, $7, $8);)";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);

    params_t expectedParams;
    expectedParams.Append("123").Append("John").Append("admin").Append(1).Append("456").Append("Jack").Append("admin2").Append(1);

    EXPECT_EQ(query->ExtractParams(), expectedParams);
}

TEST_F(InsertQuerySerializationFixture, ThrowWithInvalidTableInQueryOptions)
{
    auto options = std::make_unique<InsertOptions>();

    options->columns = {Column::login, Column::name, Column::password_hashed, Column::created_by};

    EXPECT_ANY_THROW((static_cast<IQueryOptions&>(*options)).SerializeParametrized());
}

TEST_F(InsertQuerySerializationFixture, QueryIdentificator)
{
    auto options = std::make_unique<InsertOptions>();
    InsertValues values;

    options->table = Table::User;
    options->columns = {Column::login, Column::name, Column::password_hashed, Column::created_by};

    params_t values1;
    values1.Append("123").Append("John").Append("admin").Append(1);
    values.values.emplace_back(std::move(values1));

    params_t values2;
    values2.Append("456").Append("Jack").Append("admin2").Append(1);
    values.values.emplace_back(std::move(values2));

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    EXPECT_EQ(query->GetIdentificator(), QueryIdentificator::INSERT);
    EXPECT_EQ(query->ExtractOptions()->GetIdentificator(), QueryIdentificator::INSERT);
}
