#ifndef YAZF_HPP
#define YAZF_HPP
#include "PlatformHeader.hpp"
#include <vector>
#include "File.hpp"
#include <fstream>
#include <filesystem>
namespace YAZF {
    class Yazf {
            std::vector<File> FileList;
            Yazf(const std::string& _64byteskey, const std::string& _32bitkey);
            const std::string _64bytesKey, _32bitKey;
            static std::string RandomKey(size_t SizeInBytes);
        public:
            static YAZF_API Yazf NewWithRandomEncryptionKey();
            static YAZF_API Yazf NewWithPredefinedKey(const std::string& _512byteskey, const std::string _32bitkey);
            void YAZF_API Pushfile(const File& file);
            void YAZF_API Write(std::filesystem::path Path);
            static YAZF_API Yazf Parse(std::filesystem::path Path);
    };
}
#endif