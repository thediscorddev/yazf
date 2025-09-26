#include "../Header/Util.hpp"
#include "../Header/File.hpp"
#include "../Header/Meta_Tags.hpp"
#include <map>
using namespace YAZF;
std::string Util::BuildPathsTable()
{
    std::map<std::string, std::string> Map;
    int i = 0;
    for(const auto & el: File::GlobalFiles)
    {
        if(Map.count(el.Path) == 0)
        {
            Map[el.Path] = std::to_string(i++);
        }
    }
    std::string content;
    content += Meta_Tags::PATHTABLE_BEGIN;
    for(auto & el: Map)
    {
        content += "\n";
        content += "\""+el.first+"\": " + el.first;
    }
    content += "\n";
    content += Meta_Tags::PATHTABLE_END;
    return content;
}
std::string Util::BuildFilesTable()
{
    std::string content;
    content += Meta_Tags::FILETABLE_BEGIN;
    for(auto & el: File::GlobalFiles)
    {
        content += "\n";
        content += el.GetFileInformation();
    }
    content += "\n";
    content += Meta_Tags::FILETABLE_END;
    return content;
}