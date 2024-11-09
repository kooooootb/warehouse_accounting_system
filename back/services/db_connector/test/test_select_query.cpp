#include <gtest/gtest.h>

#include <tracer/mock/tracer_mock.h>

#include <db_connector/product_definitions/columns.h>
#include <db_connector/product_definitions/tables.h>
#include <db_connector/query/condition.h>
#include <db_connector/query/query_factory.h>
#include <db_connector/query/select_query_params.h>

using namespace srv::db;

class SelectQuerySerializationFixture : public testing::Test
{
public:
    SelectQuerySerializationFixture() : m_tracerMock(srv::mock::TracerMock::Create()) {}

protected:
    std::shared_ptr<srv::ITracer> m_tracerMock;
};

TEST_F(SelectQuerySerializationFixture, HP)
{
    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::User;
    options->columns = {Column::user_id, Column::name, Column::created_by};

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString = "SELECT user_id, name, created_by FROM public.\"User\";";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);
}

TEST_F(SelectQuerySerializationFixture, All)
{
    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::User;

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString = "SELECT * FROM public.\"User\";";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);
}

TEST_F(SelectQuerySerializationFixture, WithJoin)
{
    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::User;
    options->columns = {Column::user_id, Column::name, Column::main_color};

    Join join;
    join.leftColumn = Column::user_id;
    join.joiningColumn = Column::created_by;
    join.joiningTable = Table::Product;
    join.type = JoinType::INNER;

    options->joins.emplace_back(std::move(join));

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString =
        "SELECT user_id, name, main_color FROM public.\"User\" INNER JOIN Product ON User.user_id=Product.created_by;";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);
}

TEST_F(SelectQuerySerializationFixture, WithCondition)
{
    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::User;
    options->columns = {Column::user_id, Column::name};

    auto real1 = CreateRealCondition(Column::user_id, 123, RealConditionType::Greater);
    auto real2 = CreateRealCondition(Column::name, "John", RealConditionType::Equal);
    auto real3 = CreateRealCondition(Column::login, "Jack", RealConditionType::NotEqual);

    auto not1 = std::make_unique<NotCondition>();
    not1->condition = std::move(real3);

    auto group1 = CreateGroupCondition(GroupConditionType::AND);
    group1->conditions.emplace_back(std::move(real1));
    group1->conditions.emplace_back(std::move(real2));

    auto group2 = CreateGroupCondition(GroupConditionType::OR);
    group2->conditions.emplace_back(std::move(group1));
    group2->conditions.emplace_back(std::move(not1));

    options->condition = std::move(group2);

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    std::string_view queryString =
        R"(SELECT user_id, name FROM public."User" WHERE (((user_id > $1) AND (name = $2)) OR (NOT (login != $3)));)";

    EXPECT_EQ(query->ExtractOptions()->SerializeParametrized(), queryString);

    params_t expectedParams;
    expectedParams.Append(123).Append("John").Append("Jack");

    EXPECT_EQ(query->ExtractParams(), expectedParams);
}

TEST_F(SelectQuerySerializationFixture, QueryIdentificator)
{
    auto options = std::make_unique<SelectOptions>();
    SelectValues values;

    options->table = Table::User;
    options->columns = {Column::user_id, Column::name, Column::main_color};

    const auto query = QueryFactory::Create(m_tracerMock, std::move(options), std::move(values));

    EXPECT_EQ(query->GetIdentificator(), QueryIdentificator::SELECT);
    EXPECT_EQ(query->ExtractOptions()->GetIdentificator(), QueryIdentificator::SELECT);
}
