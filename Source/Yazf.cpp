#include "../Header/Yazf.hpp"
#include <random>
#include <iostream>
#include <stdexcept>
#include "../Header/DecryptionAndEncryption.hpp"
#include "../Header/Util.hpp"
#include "../Header/Meta_Tags.hpp"
#include <string>
#include "../Header/KeyAndContentPairContainer.hpp"
using namespace YAZF;
Yazf::Yazf(const std::string &_32byteskey, const std::string &_4byteskey)
    : _32bytesKey(_32byteskey), _4bytesKey(_4byteskey)
{
}
std::string Yazf::RandomKey(size_t SizeInBytes)
{
    std::string content = "";
    const std::string &predefinedchar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-=`~_+{}[];'|,./<>?/*-+";
    //
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, predefinedchar.size() - 1);

    std::string s;
    content.reserve(SizeInBytes);
    for (std::size_t i = 0; i < SizeInBytes; ++i)
    {
        content.push_back(predefinedchar[dist(gen)]);
    }
    return content;
}
Yazf Yazf::NewWithRandomEncryptionKey()
{
    return NewWithPredefinedKey(RandomKey(ENCRYPTIONKEYSIZE), RandomKey(4));
}
Yazf Yazf::NewWithPredefinedKey(const std::string &_32byteskey, const std::string _4byteskey)
{
    std::string _32bytesKey = _32byteskey;
    std::string _4bytesKey = _4byteskey;
    if (_32bytesKey.size() < ENCRYPTIONKEYSIZE)
    {
        _32bytesKey.resize(ENCRYPTIONKEYSIZE, '\0');
    }
    else if (_32byteskey.size() > ENCRYPTIONKEYSIZE)
    {
        _32bytesKey.resize(ENCRYPTIONKEYSIZE);
    }
    if (_4byteskey.size() < 4)
    {
        _4bytesKey.resize(4, '\0');
    }
    else if (_4byteskey.size() > 4)
    {
        _4bytesKey.resize(4);
    }
    return Yazf(_32byteskey, _4byteskey);
}
void Yazf::Pushfile(const File &file)
{
    FileList.push_back(file);
}
void Yazf::Write(std::filesystem::path Path)
{
    std::ofstream output(Path, std::ios::out | std::ios::binary);
    if (!output.is_open())
    {
        throw std::runtime_error("Failed to open file.");
    }
    // preparing files
    std::string Extended4BytesKey = Form32BytesKey(_32bytesKey, _4bytesKey);
    File::GlobalFiles.insert(File::GlobalFiles.end(), FileList.begin(), FileList.end());
    std::string iv = Getiv(_32bytesKey);
    std::string EncryptedPathsTable = "";
    DecryptionAndEncryption::Encrypt(Extended4BytesKey, iv, Util::BuildPathsTable(), EncryptedPathsTable);
    DecryptionAndEncryption::Encrypt(_32bytesKey, iv, EncryptedPathsTable, EncryptedPathsTable);
    // write the first 64 bytes encryption key
    output.seekp(0);
    output.write(_32bytesKey.data(), _32bytesKey.size());
    // write the header next
    // write the encrypted path table next
    output.seekp((ENCRYPTIONKEYSIZE + HEADERSIZE));
    output.write(EncryptedPathsTable.data(), EncryptedPathsTable.size());

    // write the file content next
    // but first, we will encrypt them first
    std::map<std::string, std::pair<std::string, unsigned long long int>> EncryptedFileContent;
    std::map<std::pair<std::pair<std::string, std::string>, std::string>, std::pair<std::string, unsigned long long int>> EncryptedFileContentWithSpecialKey;
    for (unsigned long long int i = 0; i < File::GlobalFiles.size(); i++)
    {
        const auto &file = File::GlobalFiles[i];
        std::string UniqueKey = RandomKey(16);
        std::string UsedUniqueKey = UniqueKeyTo32BytesKey(UniqueKey);
        std::string EncryptedUniqueKey = UniqueKey;
        // Encrypt the encryption key
        DecryptionAndEncryption::Encrypt(Extended4BytesKey, iv, EncryptedUniqueKey, EncryptedUniqueKey);
        DecryptionAndEncryption::Encrypt(_32bytesKey, iv, EncryptedUniqueKey, EncryptedUniqueKey);
        // we now have the first std::pair<string, string>
        // now we will encrypt the content with the nonencrypted unique key
        std::string Content = file.GetFileContent();
        std::string EncryptedContent = Content;
        DecryptionAndEncryption::Encrypt(UsedUniqueKey, iv, EncryptedContent, EncryptedContent);
        DecryptionAndEncryption::Encrypt(Extended4BytesKey, iv, EncryptedContent, EncryptedContent);
        DecryptionAndEncryption::Encrypt(_32bytesKey, iv, EncryptedContent, EncryptedContent);
        // now we will push element
        std::pair<std::string, std::string> UniqueKeyPair(EncryptedUniqueKey, UniqueKey);
        std::pair<std::pair<std::string, std::string>, std::string> UniqueKeyPairAndContent(UniqueKeyPair, Content);
        std::pair<std::string, unsigned long long int> EncryptedContentWithIndex(EncryptedContent, i);
        EncryptedFileContentWithSpecialKey[UniqueKeyPairAndContent] = EncryptedContentWithIndex;
        EncryptedFileContent[Content] = EncryptedContentWithIndex;
    }
    size_t extraIndex = 0;
    for (const auto &el : EncryptedFileContentWithSpecialKey)
    {
        const auto &First = el.first;
        const auto &Second = el.second;
        const auto &EncryptedUniqueKey = First.first.first;
        const auto &EncryptedContent = Second.first;
        output.seekp((ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex);
        output.write(EncryptedUniqueKey.data(), EncryptedUniqueKey.size());
        extraIndex += 16;
        output.seekp((ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex);
        output.write(EncryptedContent.data(), EncryptedContent.size());
        extraIndex += Util::SizeOfString(EncryptedContent);
    }
    std::string EncryptedFileTable = Util::BuildFilesTable(EncryptedPathsTable, EncryptedFileContent);
    DecryptionAndEncryption::Encrypt(Extended4BytesKey, iv, EncryptedFileTable, EncryptedFileTable);
    DecryptionAndEncryption::Encrypt(_32bytesKey, iv, EncryptedFileTable, EncryptedFileTable);
    std::cout << "File table size: " << EncryptedFileTable.size() << std::endl;
    std::cout << "hex Of EncryptedFile Table write:";
    Util::printhex(EncryptedFileTable);
    output.seekp((ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex);
    std::cout << "File table really started at: " << (ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex << std::endl;
    output.write(EncryptedFileTable.data(), EncryptedFileTable.size());
    // we have done writing files table
    // Finally, we will write our header
    // Header will only got encrypted by the 512 bits key
    std::string EncryptedHeader = Util::BuildHeader(_4bytesKey, EncryptedPathsTable, EncryptedFileTable, EncryptedFileContent);
    DecryptionAndEncryption::EncryptWithPadding(_32bytesKey, iv, EncryptedHeader, 1024, EncryptedHeader);
    output.seekp(ENCRYPTIONKEYSIZE);
    output.write(EncryptedHeader.data(), EncryptedHeader.size());
    output.close();
    File::GlobalFiles.clear();
    FileList.clear();
}
Yazf Yazf::Parse(std::filesystem::path Path)
{
    // Preparing the vector Header
    KeyAndContentPairContainer HeaderDataList;
    std::ifstream input(Path, std::ios::binary);
    if (!input)
    {
        std::cerr << "Failed to open file\n";
        return NewWithRandomEncryptionKey();
    }
    input.seekg(0, std::ios::beg);
    char _32bytesKey[ENCRYPTIONKEYSIZE + 1] = {0};
    input.read(_32bytesKey, ENCRYPTIONKEYSIZE);
    std::string iv = Getiv(_32bytesKey);
    iv.resize(16);
    char EncryptedHeader[HEADERSIZE + 1] = {0};
    input.seekg(ENCRYPTIONKEYSIZE, std::ios::beg);
    input.read(EncryptedHeader, HEADERSIZE);
    std::string EncryptedHeaderStr(EncryptedHeader, HEADERSIZE);
    DecryptionAndEncryption::Decrypt(_32bytesKey, iv, EncryptedHeaderStr, EncryptedHeaderStr);
    // Parse Meta Data
    std::vector<std::string> SplitedMetaData;
    Util::SplitString(EncryptedHeaderStr, '\n', SplitedMetaData);
    if (SplitedMetaData.size() <= 2)
        throw std::runtime_error("Corrupted Meta Data");
    if (std::string(SplitedMetaData[0]) != Meta_Tags::METADATA_BEGIN)
        throw std::runtime_error("Invaild Meta Data");
    for (int i = 1; i < SplitedMetaData.size() - 1; i++)
    {
        std::vector<std::string> SplitedKey;
        Util::SplitString(SplitedMetaData[i], ':', SplitedKey);
        if (SplitedKey.size() < 2)
            throw std::runtime_error("Invaild key: " + SplitedMetaData[i]);
        std::string AccessKey(SplitedKey[0]);
        std::string Content = "";
        for (int i = 1; i < SplitedKey.size(); i++)
        {
            Content += SplitedKey[i];
        }
        // we will split the " out
        SplitedKey.clear();
        Util::SplitString(AccessKey, '"', SplitedKey);
        if (SplitedKey.size() < 3)
            throw std::runtime_error("Invaild Access Key");
        AccessKey = "";
        for (int i = 1; i < SplitedKey.size(); i++)
        {
            AccessKey += SplitedKey[i];
        }
        SplitedKey.clear();
        Util::SplitString(Content, '"', SplitedKey);
        Content = "";
        if (SplitedKey.size() == 1)
            Content = SplitedKey[0]; // a number
        if (SplitedKey.size() == 0 || SplitedKey.size() == 2)
            throw std::runtime_error("Invaild Content");
        for (int i = 1; i < SplitedKey.size(); i++)
        {
            Content += SplitedKey[i];
        }
        //
        HeaderDataList.append(AccessKey, Content);
    }
    for (const auto &el : HeaderDataList.GetList())
    {
        std::cout << el.GetFirst() << ": " << el.GetSecond() << " (" << el.GetContentType() << ")" << std::endl;
    }
    // parse the path table
    unsigned long long int PathTableBegin = std::stoull(HeaderDataList.GetOrThrow("FilePathsTableBegin", "Invaild meta data").GetSecond());
    unsigned long long int PathTableEnd = std::stoull(HeaderDataList.GetOrThrow("FilePathsTableEnd", "Invaild meta data").GetSecond());
    unsigned long long int PathTableSize = PathTableEnd - PathTableBegin + 1;
    std::string _4bytesEncryptionKey = HeaderDataList.GetOrThrow("EncryptionKey", "Invaild meta data").GetSecond();
    std::string Extended4BytesKey = Form32BytesKey(_32bytesKey, _4bytesEncryptionKey);
    std::vector<char> EncryptedPathsTable(PathTableSize + 1, 0);
    input.seekg(PathTableBegin, std::ios::beg);
    input.read(EncryptedPathsTable.data(), PathTableSize);
    std::string test1;
    std::string DecryptedPathTable(EncryptedPathsTable.data(), PathTableSize);
    std::cout << DecryptionAndEncryption::Decrypt(_32bytesKey, iv, DecryptedPathTable, DecryptedPathTable) << std::endl;
    std::cout << DecryptionAndEncryption::Decrypt(Extended4BytesKey, iv, DecryptedPathTable, DecryptedPathTable) << std::endl;
    // Parse Paths Table
    // Parse files Table
    unsigned long long int FileTableBegin = std::stoull(HeaderDataList.GetOrThrow("FilesTableBegin", "Invaild meta data").GetSecond());
    unsigned long long int FileTableEnd = std::stoull(HeaderDataList.GetOrThrow("FilesTableEnd", "Invaild meta data").GetSecond());
    unsigned long long int FileTableSize = FileTableEnd - FileTableBegin + 1;
    std::cout << "FileTableSize: " << FileTableSize << std::endl;
    std::vector<char> EncryptedFilesTable(FileTableSize + 1, 0);
    input.seekg(FileTableBegin, std::ios::beg);
    input.read(EncryptedFilesTable.data(), FileTableSize);
    std::string DecryptedFileTable(EncryptedFilesTable.data(), FileTableSize);
    std::cout << "hex Of EncryptedFile Table:";
    Util::printhex(DecryptedFileTable);
    std::cout << DecryptionAndEncryption::Decrypt(_32bytesKey, iv, DecryptedFileTable, DecryptedFileTable) << std::endl;
    std::cout << DecryptionAndEncryption::Decrypt(Extended4BytesKey, iv, DecryptedFileTable, DecryptedFileTable) << std::endl;
    std::cout << std::endl << DecryptedFileTable<<std::endl;
    input.close();
    return NewWithRandomEncryptionKey();
}
std::string Yazf::Getiv(const std::string &_32byteskey)
{
    return std::string(_32byteskey.begin(), _32byteskey.begin() + 16);
}
std::string Yazf::Form32BytesKey(const std::string &_32byteskey, const std::string &_4bytesKey)
{
    std::string str1(_32byteskey.begin(), _32byteskey.begin() + 4);
    std::string str2(_32byteskey.begin() + 6, _32byteskey.begin() + 10);
    std::string str3(_32byteskey.begin() + 11, _32byteskey.begin() + 15);
    std::string str4(_32byteskey.begin() + 16, _32byteskey.begin() + 20);
    std::string str5(_32byteskey.begin() + 22, _32byteskey.begin() + 26);
    return str2 + _4bytesKey + str1 + str3 + _4bytesKey + str4 + _4bytesKey + str5;
}
std::string Yazf::UniqueKeyTo32BytesKey(const std::string UniqueKey)
{
    const std::string str1(UniqueKey.begin(), UniqueKey.begin() + 4);
    const std::string str2(UniqueKey.begin() + 4, UniqueKey.begin() + 8);
    const std::string str3(UniqueKey.begin() + 8, UniqueKey.begin() + 12);
    const std::string str4(UniqueKey.begin() + 12, UniqueKey.end());
    return str4 + str2 + str3 + str1 + str1 + str3 + str2 + str4;
}