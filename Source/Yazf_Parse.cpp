#include "../Header/Yazf.hpp"
#include <random>
#include <iostream>
#include <stdexcept>
#include "../Header/DecryptionAndEncryption.hpp"
#include "../Header/Util.hpp"
#include "../Header/Meta_Tags.hpp"
#include <string>
#include "../Header/KeyAndContentPairContainer.hpp"
#include "../Header/Debug.hpp"
using namespace YAZF;
Yazf Yazf::Parse(std::filesystem::path Path)
{
    // Preparing the vector Header
    KeyAndContentPairContainer HeaderDataList, PathList;
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
    // Trip the extra paddling 0
    Util::RemoveTrailingFromString(SplitedMetaData[SplitedMetaData.size() - 1]);
    if (std::string(SplitedMetaData[SplitedMetaData.size() - 1]) != Meta_Tags::METADATA_END)
        throw std::runtime_error("Invaild Meta Data");
    for (int i = 1; i < SplitedMetaData.size() - 1; i++)
    {
        Util::ParseContentKeyAndData(HeaderDataList, SplitedMetaData[i]);
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
    DecryptionAndEncryption::Decrypt(_32bytesKey, iv, DecryptedPathTable, DecryptedPathTable);
    DecryptionAndEncryption::Decrypt(Extended4BytesKey, iv, DecryptedPathTable, DecryptedPathTable);
    // Parse Paths Table
    std::vector<std::string> SplitedPathTable;
    Util::SplitString(DecryptedPathTable, '\n', SplitedPathTable);
    if (SplitedPathTable.size() < 2)
        throw std::runtime_error("Corrupted paths table");
    if (std::string(SplitedPathTable[0]) != Meta_Tags::PATHTABLE_BEGIN)
        throw std::runtime_error("Invaild paths table");
    if (std::string(SplitedPathTable[SplitedPathTable.size() - 1]) != Meta_Tags::PATHTABLE_END)
        throw std::runtime_error("Invaild paths table");
    for (unsigned long long int i = 1; i < SplitedPathTable.size() - 1; i++)
    {
        Util::ParseContentKeyAndData(PathList, SplitedPathTable[i]);
    }
    // Parse files Table
    unsigned long long int FileTableBegin = std::stoull(HeaderDataList.GetOrThrow("FilesTableBegin", "Invaild meta data").GetSecond());
    unsigned long long int FileTableEnd = std::stoull(HeaderDataList.GetOrThrow("FilesTableEnd", "Invaild meta data").GetSecond());
    unsigned long long int FileTableSize = FileTableEnd - FileTableBegin + 1;
    std::vector<char> EncryptedFilesTable(FileTableSize + 1, 0);
    input.seekg(FileTableBegin, std::ios::beg);
    input.read(EncryptedFilesTable.data(), FileTableSize);
    std::string DecryptedFileTable(EncryptedFilesTable.data(), FileTableSize);
    DecryptionAndEncryption::Decrypt(_32bytesKey, iv, DecryptedFileTable, DecryptedFileTable);
    DecryptionAndEncryption::Decrypt(Extended4BytesKey, iv, DecryptedFileTable, DecryptedFileTable);
    // After decrypted file table, we will parse it
    std::vector<std::string> SplitedFileTable;
    Util::SplitString(DecryptedFileTable, '\n', SplitedFileTable);
    if (SplitedFileTable.size() < 2)
        throw std::runtime_error("Corrupted Files table");
    if (std::string(SplitedFileTable[0]) != Meta_Tags::FILETABLE_BEGIN)
        throw std::runtime_error("Invaild Files Table Data");
    // Trip the extra paddling 0
    Util::RemoveTrailingFromString(SplitedFileTable[SplitedFileTable.size() - 1]);
    if (std::string(SplitedFileTable[SplitedFileTable.size() - 1]) != Meta_Tags::FILETABLE_END)
        throw std::runtime_error("Invaild Files Table Data");
    File CurrentFile;
    KeyAndContentPairContainer CurrentFileData;
    for (unsigned long long int i = 1; i < SplitedFileTable.size() - 1; i++)
    {
        const std::string &Line = SplitedFileTable[i];
        if (Line == Meta_Tags::FILEINFORMATION_BEGIN)
        {
            CurrentFile = File();
            CurrentFileData.GetList().clear();
            continue;
        }
        if (Line == Meta_Tags::FILEINFORMATION_END)
        {
            // parsing logic
            std::string FileName = CurrentFileData.GetOrThrow("Name", "Invaild File Information").GetSecond();
            std::string FilePathId = CurrentFileData.GetOrThrow("Name", "Invaild File Information").GetSecond();
            // std::string FilePathRelative = PathList.GetOrThrow(FilePathId, "Invaild File Path").GetSecond();
            unsigned long long int ContentOffset1 = std::stoull(CurrentFileData.GetOrThrow("ContentOffset1", "Invaild File Information").GetSecond());
            unsigned long long int ContentOffset2 = std::stoull(CurrentFileData.GetOrThrow("ContentOffset2", "Invaild File Information").GetSecond());
            // read file
            char EncryptedUniqueKey[ENCRYPTEDUNIQUEKEYSIZE + 1] = {0};
            input.seekg(ContentOffset1, std::ios::beg);
            input.read(EncryptedUniqueKey, ENCRYPTEDUNIQUEKEYSIZE);
            std::string DecryptedUniqueKey(EncryptedUniqueKey, ENCRYPTEDUNIQUEKEYSIZE);
            Debug::DelayedPrint::PushToBufferAndEndLine("UniqueKey in file:");
            Debug::DelayedPrint::PushToBufferAndEndLine(Util::toHex(DecryptedUniqueKey).c_str());
            DecryptionAndEncryption::Decrypt(_32bytesKey, iv, DecryptedUniqueKey, DecryptedUniqueKey);
            DecryptionAndEncryption::Decrypt(Extended4BytesKey, iv, DecryptedUniqueKey, DecryptedUniqueKey);
            unsigned long long int ContentSize = ContentOffset2 - ContentOffset1 - ENCRYPTEDUNIQUEKEYSIZE;
            input.seekg(ContentOffset1 + ENCRYPTEDUNIQUEKEYSIZE, std::ios::beg);
            std::vector<char> EncryptedContent(ContentSize + 1, 0);
            input.read(EncryptedContent.data(), ContentSize);
            std::string DecryptedContent(EncryptedContent.data(), ContentSize);
            Debug::DelayedPrint::PushToBufferAndEndLine("File Content hex in file: ");
            Debug::DelayedPrint::PushToBufferAndEndLine(Util::toHex(DecryptedContent).c_str());
            DecryptionAndEncryption::Decrypt(_32bytesKey, iv, DecryptedContent, DecryptedContent);
            DecryptionAndEncryption::Decrypt(Extended4BytesKey, iv, DecryptedContent, DecryptedContent);
            DecryptionAndEncryption::Decrypt(UniqueKeyTo32BytesKey(DecryptedUniqueKey), iv, DecryptedContent, DecryptedContent);
            Debug::DelayedPrint::PushToBuffer("Decrypted data: ");
            Debug::DelayedPrint::PushToBufferAndEndLine(DecryptedContent.c_str());

            for (const auto &el : CurrentFileData.GetList())
            {
                Debug::DelayedPrint::PushToBufferAndEndLine(std::string(el.GetFirst() + ": " + el.GetSecond() + " (" + el.GetContentType() + ")").c_str());
            }
            continue;
        }
        // reading logic
        Util::ParseContentKeyAndData(CurrentFileData, SplitedFileTable[i]);
    }
    Debug::DelayedPrint::PushToBufferAndEndLine(DecryptedFileTable.c_str());
    input.close();
    Debug::DelayedPrint::PrintAndFlushBuffer();
    return NewWithRandomEncryptionKey();
}