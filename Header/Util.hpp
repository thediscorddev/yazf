#ifndef UTIL_HPP
#define UTIL_HPP
#include "PlatformHeader.hpp"
#include <map>
#include <string>
#include <utility>
#include <vector>
namespace YAZF {
    class Util {
        public:
            static YAZF_API unsigned long int SizeOfString(const std::string& Content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const std::string& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const int& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const unsigned int& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const unsigned long int& content);
            static YAZF_API std::string FormContentKey(const std::string& Key, const long int& content);
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
            static YAZF_API std::string BuildHeader(const std::string& _32bitEncryptionKey, const std::string& EncryptedPathsTable, const std::string& EncryptedFilesTable, const std::map<std::string, std::pair<std::string, unsigned int>>& EncryptedFileContent);
            static YAZF_API std::string BuildFilesTable(const std::string& EncryptedPathTable, const std::map<std::string, std::pair<std::string, unsigned int>>& EncryptedFileContent);
    };
}
#endif