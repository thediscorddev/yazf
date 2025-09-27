#include "../Header/Util.hpp"
#include "../Header/File.hpp"
#include "../Header/Meta_Tags.hpp"
#include <map>
#include <algorithm>
#include <vector>
using namespace YAZF;
unsigned long int Util::SizeOfString(const std::string& Content)
{
    return Content.size() * sizeof(char);
}
std::string Util::FormContentKey(const std::string& Key, const std::string& content)
{
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content+= "\"";
    Content+= content;
    Content+= "\"";
    return Content;
}
std::string Util::FormContentKey(const std::string& Key, const int& content)
{
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content+= std::to_string(content);
    return Content;
}
std::string Util::FormContentKey(const std::string& Key, const double& content){
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content+= std::to_string(content);
    return Content;
};
std::string Util::FormContentKey(const std::string& Key, const float& content){
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content+= std::to_string(content);
    return Content; 
};
std::string Util::FormContentKey(const std::string& Key, const std::vector<std::string>& content){
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content += "[";
    for(size_t i = 0; i < content.size(); i++)
    {
        const auto& el = content[i];
        if(i != 0) Content += ", ";
        Content+= "\"";
        Content+= el;
        Content+= "\"";
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string& Key, const std::vector<int>& content){
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content += "[";
    for(size_t i = 0; i < content.size(); i++)
    {
        const auto& el = content[i];
        if(i != 0) Content += ", ";
        Content+= std::to_string(el);
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string& Key, const std::vector<double>& content){
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content += "[";
    for(size_t i = 0; i < content.size(); i++)
    {
        const auto& el = content[i];
        if(i != 0) Content += ", ";
        Content+= std::to_string(el);
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string& Key, const std::vector<float>& content){
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content += "[";
    for(size_t i = 0; i < content.size(); i++)
    {
        const auto& el = content[i];
        if(i != 0) Content += ", ";
        Content+= std::to_string(el);
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string& Key, const std::vector<unsigned int>& content)
{
    std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content += "[";
    for(size_t i = 0; i < content.size(); i++)
    {
        const auto& el = content[i];
        if(i != 0) Content += ", ";
        Content+= std::to_string(el);
    }
    Content += "]";
    return Content;
}
std::string Util::FormContentKey(const std::string& Key, const std::vector<long int>& content){
       std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content += "[";
    for(size_t i = 0; i < content.size(); i++)
    {
        const auto& el = content[i];
        if(i != 0) Content += ", ";
        Content+= std::to_string(el);
    }
    Content += "]";
    return Content; 
};
std::string Util::FormContentKey(const std::string& Key, const unsigned int& content){
        std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content+= std::to_string(content);
    return Content;
};
std::string Util::FormContentKey(const std::string& Key, const unsigned long int& content){
        std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content+= std::to_string(content);
    return Content;
};
std::string Util::FormContentKey(const std::string& Key, const long int& content){
        std::string Content;
    Content+= "\"";
    Content+= Key;
    Content+= "\"";
    Content+= ": ";
    Content+= std::to_string(content);
    return Content;
};
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
std::string Util::BuildFilesTable(const std::string& EncryptedPathTable, const std::map<std::string, std::pair<std::string, unsigned int>>& EncryptedFileContent)
{

    const unsigned int index = ENCRYPTIONKEYSIZE + HEADERSIZE;
    std::vector<std::pair<std::string, unsigned int>> SortedEncryptedContent;
    for(const auto& el: EncryptedFileContent)
    {
        SortedEncryptedContent.push_back(el.second);
    }
    std::sort(SortedEncryptedContent.begin(), SortedEncryptedContent.end(), 
    [](const auto& first, const auto& second) {
        return first.second < second.second;
    });
    // We begin with 0 and not 1
    std::string content;
    content += Meta_Tags::FILETABLE_BEGIN;
    for(auto & el: File::GlobalFiles)
    {
        content += "\n";
        const std::string& BaseEncryptedContent = EncryptedFileContent.at(el.GetFileContent()).first; 
        unsigned int FileIndex1 = 0;
        unsigned int FileIndex2 = 0;
        for(int i = EncryptedFileContent.at(el.GetFileContent()).second-1; i >= 0; i--)
        {
            const std::string& EncryptedContent = SortedEncryptedContent[i].first; 
            FileIndex1+= SizeOfString(EncryptedContent) + 16;
        }
        FileIndex2+=FileIndex1+ SizeOfString(BaseEncryptedContent) + 16;
        content += el.GetFileInformation(index+FileIndex1*8, index+FileIndex2*8);
    }
    content += "\n";
    content += Meta_Tags::FILETABLE_END;
    return content;
}

std::string Util::BuildHeader(const std::string& _32bitEncryptionKey, const std::string& EncryptedPathsTable, const std::string& EncryptedFilesTable, const std::map<std::string, std::pair<std::string, unsigned int>>& EncryptedFileContent)
{
    std::string Content = "";
    Content += Meta_Tags::METADATA_BEGIN;
    Content += "\n";
    Content += Util::FormContentKey("FileVersion",YAZF_VERSION) + "\n";
    Content += Util::FormContentKey("EncryptionKey",_32bitEncryptionKey) + "\n";
    Content += Util::FormContentKey("FilePathsTableBegin",ENCRYPTIONKEYSIZE + HEADERSIZE+1) + "\n";
    Content += Util::FormContentKey("FilePathsTableEnd",ENCRYPTIONKEYSIZE + HEADERSIZE+SizeOfString(EncryptedPathsTable)*8) + "\n";
    // Calculating content size
    unsigned long int ContentSize = 0;
    for(auto& el: EncryptedFileContent)
    {
        ContentSize += 128+SizeOfString(el.first)*8;
    }
    Content += Util::FormContentKey("FilesTableBegin",ENCRYPTIONKEYSIZE + HEADERSIZE+SizeOfString(EncryptedPathsTable)*8+1+ContentSize) + "\n";
    Content += Util::FormContentKey("FilesTableEnd",ENCRYPTIONKEYSIZE + HEADERSIZE+SizeOfString(EncryptedPathsTable)*8+ContentSize+SizeOfString(EncryptedFilesTable)*8) + "\n";
    Content += Util::FormContentKey("FileContentsBegin",ENCRYPTIONKEYSIZE + HEADERSIZE+SizeOfString(EncryptedPathsTable)*8+1) + "\n";
    Content += Util::FormContentKey("FileContentsEnd",ENCRYPTIONKEYSIZE + HEADERSIZE+SizeOfString(EncryptedPathsTable)*8+ContentSize) + "\n";
    Content += Meta_Tags::METADATA_END;
    return Content;
}