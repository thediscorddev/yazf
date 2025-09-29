#ifndef UTIL_HPP
#define UTIL_HPP
#include "PlatformHeader.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "KeyAndContentPairContainer.hpp"
namespace YAZF {
    class Util {
            static YAZF_API std::map<std::string, std::string> GetAllPath();
            static YAZF_API void RegexOrThrow(const std::string& content);
        public:
            static YAZF_API void ParseContentKeyAndData(KeyAndContentPairContainer& container, const std::string& TargetKey);
            static YAZF_API void SplitString(const std::string& s, char delimiter, std::vector<std::string>& tokens);
            static YAZF_API unsigned long long int SizeOfString(const std::string& Content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::string& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const int& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const unsigned long long int& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const long long int& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const double& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const float& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::vector<std::string>& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::vector<int>& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::vector<unsigned int>& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::vector<long int>& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::vector<double>& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::vector<float>& content);

            static YAZF_API std::string BuildPathsTable();
            //static YAZF_API std::string BuildPathsTableWithExtraFolder();
            static YAZF_API std::string BuildHeader(const std::string& _32bitEncryptionKey, const std::string& EncryptedPathsTable, const std::string& EncryptedFilesTable, const std::map<std::string, std::pair<std::string, unsigned long long int>>& EncryptedFileContent);
            static YAZF_API std::string BuildFilesTable(const std::string& EncryptedPathTable, const std::map<std::string, std::pair<std::string, unsigned long long int>>& EncryptedFileContent);
            static YAZF_API bool isInt(const std::string& s);
            static YAZF_API bool isFloat(const std::string& s);
            static YAZF_API bool isDouble(const std::string& s);
            static YAZF_API void printhex(const std::string& s);
            static YAZF_API void RemoveTrailingFromString(std::string& s);
    };
}
#endif