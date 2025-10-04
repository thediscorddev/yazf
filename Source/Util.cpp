#include "../Header/Util.hpp"
#include "../Header/File.hpp"
#include "../Header/Meta_Tags.hpp"
#include <map>
#include <algorithm>
#include <vector>
#include <regex>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace YAZF;

unsigned long long int Util::SizeOfString(const std::string &Content)
{
    return Content.size() * sizeof(char);
}
void Util::RegexOrThrow(const std::string &content)
{
    std::regex forbidden("[^a-zA-Z0-9]");
    if (std::regex_search(content, forbidden))
        throw std::runtime_error("Content contains forbidden character.");
}
std::string Util::FormContentKey(const std::string &Key, const std::string &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += "\"";
    Content += content;
    Content += "\"";
    return Content;
}
std::string Util::FormContentKey(const std::string &Key, const int &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += std::to_string(content);
    return Content;
}
std::string Util::FormContentKey(const std::string &Key, const double &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += std::to_string(content);
    return Content;
};
std::string Util::FormContentKey(const std::string &Key, const float &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += std::to_string(content);
    return Content;
};
std::string Util::FormContentKey(const std::string &Key, const std::vector<std::string> &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += "[";
    for (size_t i = 0; i < content.size(); i++)
    {
        const auto &el = content[i];
        if (i != 0)
            Content += ", ";
        Content += "\"";
        Content += el;
        Content += "\"";
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string &Key, const std::vector<int> &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += "[";
    for (size_t i = 0; i < content.size(); i++)
    {
        const auto &el = content[i];
        if (i != 0)
            Content += ", ";
        Content += std::to_string(el);
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string &Key, const std::vector<double> &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += "[";
    for (size_t i = 0; i < content.size(); i++)
    {
        const auto &el = content[i];
        if (i != 0)
            Content += ", ";
        Content += std::to_string(el);
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string &Key, const std::vector<float> &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += "[";
    for (size_t i = 0; i < content.size(); i++)
    {
        const auto &el = content[i];
        if (i != 0)
            Content += ", ";
        Content += std::to_string(el);
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string &Key, const std::vector<unsigned int> &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += "[";
    for (size_t i = 0; i < content.size(); i++)
    {
        const auto &el = content[i];
        if (i != 0)
            Content += ", ";
        Content += std::to_string(el);
    }
    Content += "]";
    return Content;
}
std::string Util::FormContentKey(const std::string &Key, const std::vector<long int> &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += "[";
    for (size_t i = 0; i < content.size(); i++)
    {
        const auto &el = content[i];
        if (i != 0)
            Content += ", ";
        Content += std::to_string(el);
    }
    Content += "]";
    return Content;
};
std::string Util::FormContentKey(const std::string &Key, const unsigned long long int &content)
{
    RegexOrThrow(Key);
    std::string Content;
    Content += "\"";
    Content += Key;
    Content += "\"";
    Content += ": ";
    Content += std::to_string(content);
    return Content;
};
std::map<std::string, std::string> Util::GetAllPath()
{
    std::map<std::string, std::string> Map;
    int i = 0;
    for (const auto &el : File::GlobalFiles)
    {
        if (Map.count(el.Path) == 0)
        {
            Map[el.Path] = std::to_string(i++);
        }
    }
    return Map;
}
std::string Util::BuildPathsTable()
{
    std::map<std::string, std::string> Map = GetAllPath();
    std::string content;
    content += Meta_Tags::PATHTABLE_BEGIN;
    for (auto &el : Map)
    {
        content += "\n";
        content += FormContentKey(el.second, el.first);
    }
    content += "\n";
    content += Meta_Tags::PATHTABLE_END;
    return content;
}
std::string Util::BuildFilesTable(const std::string &EncryptedPathTable, const std::map<std::string, std::pair<std::string, unsigned long long int>> &EncryptedFileContent)
{
    std::map<std::string, std::string> PathList = GetAllPath();
    const unsigned int index = ENCRYPTIONKEYSIZE + HEADERSIZE + SizeOfString(EncryptedPathTable);
    std::vector<std::pair<std::string, unsigned int>> SortedEncryptedContent;
    for (const auto &el : EncryptedFileContent)
    {
        SortedEncryptedContent.push_back(el.second);
    }
    std::sort(SortedEncryptedContent.begin(), SortedEncryptedContent.end(),
              [](const auto &first, const auto &second)
              {
                  return first.second < second.second;
              });
    std::cout << "Size of sorted List: " << SortedEncryptedContent.size() << std::endl;
    // We begin with 0 and not 1
    std::string content;
    content += Meta_Tags::FILETABLE_BEGIN;
    for (auto &el : File::GlobalFiles)
    {
        content += "\n";
        const std::string &BaseEncryptedContent = EncryptedFileContent.at(el.GetFileContent()).first;
        unsigned long long int FileIndex1 = 0;
        unsigned long long int FileIndex2 = 0;
        std::cout << "List after: " << EncryptedFileContent.at(el.GetFileContent()).second << std::endl;
        for (unsigned long long int i = EncryptedFileContent.at(el.GetFileContent()).second; i-- > 0;)
        {
            std::cout << i << std::endl;
            if (i >= SortedEncryptedContent.size())
                continue;
            const std::string &EncryptedContent = SortedEncryptedContent[i].first;
            FileIndex1 += SizeOfString(EncryptedContent) + ENCRYPTEDUNIQUEKEYSIZE;
        }
        FileIndex2 += FileIndex1 + SizeOfString(BaseEncryptedContent) + ENCRYPTEDUNIQUEKEYSIZE;
        content += el.GetFileInformation(index + FileIndex1, index + FileIndex2, PathList);
    }
    content += "\n";
    content += Meta_Tags::FILETABLE_END;
    std::cout << content << std::endl;
    return content;
}

std::string Util::BuildHeader(const std::string &_32bitEncryptionKey, const std::string &EncryptedPathsTable, const std::string &EncryptedFilesTable, const std::map<std::string, std::pair<std::string, unsigned long long int>> &EncryptedFileContent)
{
    std::string Content = "";
    Content += Meta_Tags::METADATA_BEGIN;
    Content += "\n";
    Content += Util::FormContentKey("FileVersion", YAZF_VERSION) + "\n";
    Content += Util::FormContentKey("EncryptionKey", _32bitEncryptionKey) + "\n";
    Content += Util::FormContentKey("FilePathsTableBegin", ENCRYPTIONKEYSIZE + HEADERSIZE) + "\n";
    Content += Util::FormContentKey("FilePathsTableEnd", ENCRYPTIONKEYSIZE + HEADERSIZE + SizeOfString(EncryptedPathsTable) - 1) + "\n";
    // Calculating content size
    unsigned long long int ContentSize = 0;
    for (auto &el : EncryptedFileContent)
    {
        ContentSize += ENCRYPTEDUNIQUEKEYSIZE + SizeOfString(el.second.first);
    }
    Content += Util::FormContentKey("FilesTableBegin", ENCRYPTIONKEYSIZE + HEADERSIZE + SizeOfString(EncryptedPathsTable) + ContentSize) + "\n";
    Content += Util::FormContentKey("FilesTableEnd", ENCRYPTIONKEYSIZE + HEADERSIZE + SizeOfString(EncryptedPathsTable) + ContentSize + SizeOfString(EncryptedFilesTable) - 1) + "\n";
    Content += Util::FormContentKey("FileContentsBegin", ENCRYPTIONKEYSIZE + HEADERSIZE + SizeOfString(EncryptedPathsTable)) + "\n";
    Content += Util::FormContentKey("FileContentsEnd", ENCRYPTIONKEYSIZE + HEADERSIZE + SizeOfString(EncryptedPathsTable) + ContentSize - 1) + "\n";
    Content += Meta_Tags::METADATA_END;
    return Content;
}
void Util::SplitString(const std::string &s, char delimiter, std::vector<std::string> &tokens)
{
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }
    if (!s.empty() && s.back() == delimiter)
    {
        tokens.push_back("");
    }
}
bool Util::isInt(const std::string &s)
{
    try
    {
        size_t idx;
        int val = std::stoi(s, &idx);
        return idx == s.size();
    }
    catch (...)
    {
        return false;
    }
}

bool Util::isFloat(const std::string &s)
{
    try
    {
        size_t idx;
        float val = std::stof(s, &idx);
        return idx == s.size();
    }
    catch (...)
    {
        return false;
    }
}
void Util::printhex(const std::string &s)
{
    for (unsigned char c : s)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(c);
    }
    std::cout << "\n";
    std::cout << std::dec;
}
bool Util::isDouble(const std::string &s)
{
    try
    {
        size_t idx;
        double val = std::stod(s, &idx);
        return idx == s.size();
    }
    catch (...)
    {
        return false;
    }
}
void Util::RemoveTrailingFromString(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char c)
                         { return c != '\0'; })
                .base(),
            s.end());
}
void Util::ParseContentKeyAndData(KeyAndContentPairContainer& container, const std::string& TargetKey)
{
        std::vector<std::string> SplitedKey;
        Util::SplitString(TargetKey, ':', SplitedKey);
        if (SplitedKey.size() < 2)
            throw std::runtime_error("Invaild key: " + TargetKey);
        std::string AccessKey(SplitedKey[0]);
        std::string Content = "";
        for (int i = 1; i < SplitedKey.size(); i++)
        {
            if(i > 1) Content += ":";
            Content += SplitedKey[i];
        }
        // we will split the " out
        SplitedKey.clear();
        Util::SplitString(AccessKey, '"', SplitedKey);
        if (SplitedKey.size() < 3)
            throw std::runtime_error("Invaild Access Key");
        AccessKey = "";
        for (int i = 1; i < SplitedKey.size()-1; i++)
        {
            if(i > 1) AccessKey += '"';
            AccessKey += SplitedKey[i];
        }
        SplitedKey.clear();
        Util::SplitString(Content, '"', SplitedKey);
        Content = "";
        if (SplitedKey.size() == 1)
            Content = SplitedKey[0]; // a number
        if (SplitedKey.size() == 0 || SplitedKey.size() == 2)
            throw std::runtime_error("Invaild Content");
        for (int i = 1; i < SplitedKey.size()-1; i++)
        {
            if(i > 1) Content += "\"";
            Content += SplitedKey[i];
        }
        container.append(AccessKey, Content);
}
std::string Util::toHex(const std::string& input_string)
{
    std::stringstream ss;
    for (char c : input_string) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    }
    return ss.str();
}