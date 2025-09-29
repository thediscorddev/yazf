#ifndef FILE_HPP
#define FILE_HPP
#include "PlatformHeader.hpp"
#include <string>
#include <vector>
#include <map>
namespace YAZF {
    class File {
        public:
            static YAZF_API std::vector<File> GlobalFiles;
            YAZF_API std::string Path = "";
            YAZF_API std::string Name = "";
            YAZF_API std::string Content = "";
            YAZF_API File(const std::string& path, const std::string& name, const std::string& content);
            YAZF_API File();
            YAZF_API std::string GetFileContent() const;
            YAZF_API std::string GetFileInformation(unsigned long long int contentoffset1, unsigned long long int contentoffset2, const std::map<std::string, std::string>& PathList) const;
    };
};
#endif