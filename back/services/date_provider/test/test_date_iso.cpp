#include <gtest/gtest.h>

#include <instrumental/types.h>
#include <locator/service_locator_mock.h>

#include "../source/date_provider.h"

using namespace srv::date;
using namespace testing;

class DateProviderFixture : public testing::Test
{
public:
    DateProviderFixture() : m_locatorMock(srv::mock::ServiceLocatorMock::Create()) {}

protected:
    std::shared_ptr<srv::mock::ServiceLocatorMock> m_locatorMock;
};

TEST_F(DateProviderFixture, Compare_Generated)
{
    DateProvider testee(m_locatorMock);

    const auto timestamp = testee.GetTimestamp();
    const auto serializedOriginal = testee.ToIsoTimeString(timestamp);

    timestamp_t reverted;
    ASSERT_EQ(testee.FromIsoTimeString(serializedOriginal, reverted), ufa::Result::SUCCESS);

    const auto serializedReverted = testee.ToIsoTimeString(reverted);

    EXPECT_EQ(serializedOriginal, serializedReverted);
}