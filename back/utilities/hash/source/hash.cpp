#include <hash/hash.h>

namespace util
{
namespace hash
{

std::string HashString(std::string_view input)
{
    std::string output;
    output.resize(SHA256_DIGEST_LENGTH);

    SHA256_CTX context;

    CHECK_TRUE(SHA256_Init(&context));

    CHECK_TRUE(SHA256_Update(&context, input.data(), input.size()));

    CHECK_TRUE(SHA256_Final(reinterpret_cast<unsigned char*>(output.data()), &context));

    return output;
}

}  // namespace hash

}  // namespace util