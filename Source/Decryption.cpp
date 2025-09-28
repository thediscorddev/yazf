#include "../Header/DecryptionAndEncryption.hpp"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>
#include <openssl/err.h>
#include <iostream>

using namespace YAZF;
bool DecryptionAndEncryption::Decrypt(
    const std::string &Key,
    const std::string &iv,
    const std::string &content,
    std::string &output)
{
    std::cout << "Key length: " << Key.size() << "\n";
    std::cout << "IV length: " << iv.size() << "\n";
    std::cout << "Ciphertext length: " << content.size() << "\n";

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        ERR_print_errors_fp(stderr);
        return false;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char *>(Key.c_str()),
                           reinterpret_cast<const unsigned char *>(iv.c_str())) != 1)
    {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> plaintext(content.size());
    int len = 0, plaintext_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                          reinterpret_cast<const unsigned char *>(content.data()),
                          content.size()) != 1)
    {
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1)
    {
        ERR_print_errors_fp(stderr); // <--- most failures happen here
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    output.assign(plaintext.begin(), plaintext.end());
    return true;
}
