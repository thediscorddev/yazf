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
            Yazf(const std::string& _32byteskey, const std::string& _4byteskey);
            const std::string _32bytesKey, _4bytesKey;
            static std::string RandomKey(size_t SizeInBytes);
            static std::string Getiv(const std::string& _32byteskey);
            static std::string Form32BytesKey(const std::string& _32byteskey, const std::string& _4bytesKey);
            static std::string UniqueKeyTo32BytesKey(const std::string UniqueKey);
        public:
            static YAZF_API Yazf NewWithRandomEncryptionKey();
            static YAZF_API Yazf NewWithPredefinedKey(const std::string& _512byteskey, const std::string _4byteskey);
            void YAZF_API Pushfile(const File& file);
            void YAZF_API Write(std::filesystem::path Path);
            static YAZF_API Yazf Parse(std::filesystem::path Path);
    };
}
#endif