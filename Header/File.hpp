#ifndef FILE_HPP
#define FILE_HPP
#include "PlatformHeader.hpp"
#include <string>
#include <vector>
namespace YAZF {
    class File {
        public:
            static std::vector<File> GlobalFiles;
            const YAZF_API std::string Path;
            const YAZF_API std::string Name;
            const YAZF_API std::string Content;
            YAZF_API File(const std::string& path, const std::string& name, const std::string& content);
            YAZF_API std::string GetFileContent() const;
            YAZF_API std::string GetFileInformation(unsigned long int contentoffset1, unsigned long int contentoffset2) const;
    };
};
#endif