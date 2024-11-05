#include <gtest/gtest.h>

#include <hash/hash.h>

using namespace util::hash;

class HashTest : public testing::Test
{
public:
    HashTest() {}

protected:
};

TEST_F(HashTest, Simple)
{
    constexpr std::string_view input = "admin";
    constexpr std::string_view expected = "C191615151114051D19181D1D1E151F11171918131C1B18181F1F1A141819181";
    auto received = HashToBase64(input);

    EXPECT_EQ(expected, received);
}
