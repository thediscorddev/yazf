#include "../Header/DecryptionAndEncryption.hpp"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>
using namespace YAZF;
bool DecryptionAndEncryption::Encrypt(const std::string &Key, const std::string &iv, const std::string &content, std::string &output)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return false;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, reinterpret_cast<const unsigned char *>(Key.c_str()), reinterpret_cast<const unsigned char *>(iv.c_str())) != 1)
        return false;

    std::vector<unsigned char> ciphertext(content.size() + AES_BLOCK_SIZE);
    int len = 0, ciphertext_len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                          reinterpret_cast<const unsigned char *>(content.data()),
                          content.size()) != 1)
        return false;
    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
        return false;
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    output = std::string(reinterpret_cast<const char *>(ciphertext.data()), ciphertext.size());
    return true;
}
bool DecryptionAndEncryption::EncryptWithPadding(const std::string &Key, const std::string &iv, const std::string &content, const unsigned int &SizeIn16bytes, std::string &output)
{
    std::string paddled = content;
    if (SizeIn16bytes % 16 != 0)
    {
        return false;
    }
    if (paddled.size() > SizeIn16bytes - 1)
    {
        return false;
    }

    // Minimum length required
    size_t min_len = SizeIn16bytes - 16;
    if (paddled.size() < min_len)
    {
        paddled.resize(min_len, '\0');
    }
    return Encrypt(Key, iv, paddled, output);
}