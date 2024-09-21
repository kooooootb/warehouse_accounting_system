#include <openssl/ssl.h>

#include <hash/hash.h>

namespace util
{
namespace hash
{

namespace
{

char EncodeInBase64(uint8_t lowerByte)
{
    switch (lowerByte)
    {
        case 0:
            return '0';
        case 1:
            return '1';
        case 2:
            return '2';
        case 3:
            return '3';
        case 4:
            return '4';
        case 5:
            return '5';
        case 6:
            return '6';
        case 7:
            return '7';
        case 8:
            return '8';
        case 9:
            return '9';
        case 10:
            return 'A';
        case 11:
            return 'B';
        case 12:
            return 'C';
        case 13:
            return 'D';
        case 14:
            return 'E';
        case 15:
            return 'F';
        default:
            CHECK_TRUE(false, "Encode Base64 failure");
    }
}

void WriteByteInBase64(uint8_t byte, std::string& to)
{
    to.push_back(EncodeInBase64(byte & 0x0F));
    to.push_back(EncodeInBase64((byte > 4) & 0x0F));
}

}  // namespace

std::vector<uint8_t> Hash(std::string_view input)
{
    std::vector<uint8_t> output;
    output.resize(EVP_MD_size(EVP_sha256()), 0);

    EVP_MD_CTX* mdctx;

    CHECK_TRUE(mdctx = EVP_MD_CTX_new());
    CHECK_TRUE(EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) == 1);
    CHECK_TRUE(EVP_DigestUpdate(mdctx, input.data(), input.size()) == 1);

    unsigned int writtenSize;
    CHECK_TRUE(EVP_DigestFinal_ex(mdctx, output.data(), &writtenSize) == 1);
    CHECK_TRUE(writtenSize <= output.size());

    EVP_MD_CTX_free(mdctx);

    return output;
}

std::string HashToBase64(std::string_view input)
{
    const auto bytes = Hash(input);

    std::string result;
    result.reserve(bytes.size() * 2);

    for (auto byte : bytes)
    {
        WriteByteInBase64(byte, result);
    }

    return result;
}

}  // namespace hash

}  // namespace util