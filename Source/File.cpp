#include "../Header/File.hpp"
#include "../Header/Meta_Tags.hpp"
using namespace YAZF;
std::vector<File> File::GlobalFiles;
File::File(const std::string& path, const std::string& name, const std::string& content)
: Path(path), Name(name), Content(content) {

}
std::string File::GetFileContent() const
{
    std::string content;
    content += Meta_Tags::FILECONTENT_BEGIN;
    content+="\n";
    content+=Content;
    content+="\n";
    content+= Meta_Tags::FILECONTENT_END;
    return content;
}
std::string File::GetFileInformation(unsigned long int contentoffset1, unsigned long int contentoffset2) const
{
    std::string content;
    content += Meta_Tags::FILEINFORMATION_BEGIN;
    content+="\n";
    content+="\"Name\": " + Name;
    content+="\n";
    content+="\"Path\": " + Path;
    content+="\n";
    content+="\"ContentOffset1\": " + contentoffset1;
    content+="\n";
    content+="\"ContentOffset2\": " + contentoffset2;
    content+="\n";
    content+= Meta_Tags::FILEINFORMATION_END;
    return content;
}