#include "../Header/DecryptionAndEncryption.hpp"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>
#include <openssl/err.h>
using namespace YAZF;
void DecryptionAndEncryption::logError(EVP_CIPHER_CTX *ctx, const std::string &Key, const std::string &iv)
{
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                           reinterpret_cast<const unsigned char *>(Key.c_str()),
                           reinterpret_cast<const unsigned char *>(iv.c_str())) != 1)
    {
        ERR_print_errors_fp(stderr);
    }
}