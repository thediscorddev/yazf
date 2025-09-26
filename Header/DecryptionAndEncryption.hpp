#ifndef DECRYPTIONANDENCRYPTION_HPP
#define DECRYPTIONANDENCRYPTION_HPP
#include "PlatformHeader.hpp"
#include <string>
namespace YAZF {
    class DecryptionAndEncryption {
        public:
            static YAZF_API bool Encrypt(const std::string& Key, const std::string& iv,const std::string& content, std::string& output);
            static YAZF_API bool Decrypt(const std::string& Key, const std::string& iv, const std::string& content, std::string& output);
            static YAZF_API bool EncryptWithPadding(const std::string& Key, const std::string& iv, const std::string& content, const unsigned int& SizeIn16bytes, std::string& output);
    };
}
#endif