#ifndef UTIL_HPP
#define UTIL_HPP
#include "PlatformHeader.hpp"
#include <string>
namespace YAZF {
    class Util {
        public:
            static YAZF_API std::string BuildPathsTable();
            static YAZF_API std::string BuildHeader(const std::string& _32bitEncryptionKey, const std::string& EncryptedPathsTable, const std::string& EncryptedFilesTable);
            static YAZF_API std::string BuildFilesTable();
    };
}
#endif