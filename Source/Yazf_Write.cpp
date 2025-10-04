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
        std::string UniqueKey = RandomUniqueKey(UNIQUEKEYSIZE);
        std::string UsedUniqueKey = UniqueKeyTo32BytesKey(UniqueKey);
        std::string EncryptedUniqueKey = UniqueKey;
        // Encrypt the encryption key
        DecryptionAndEncryption::Encrypt(Extended4BytesKey, iv, EncryptedUniqueKey, EncryptedUniqueKey);
        DecryptionAndEncryption::Encrypt(_32bytesKey, iv, EncryptedUniqueKey, EncryptedUniqueKey); 
        Debug::DelayedPrint::PushToBufferAndEndLine("Expected Encrypted Unique key:");
        Debug::DelayedPrint::PushToBufferAndEndLine(Util::toHex(EncryptedUniqueKey).c_str());
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
        Debug::DelayedPrint::PushToBuffer("Expected File encryption key written at: ");
        Debug::DelayedPrint::PushToBufferAndEndLine(std::to_string(ENCRYPTIONKEYSIZE + HEADERSIZE + Util::SizeOfString(EncryptedPathsTable) + extraIndex).c_str());
        output.seekp((ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex);
        output.write(EncryptedUniqueKey.data(), EncryptedUniqueKey.size());
        extraIndex += ENCRYPTEDUNIQUEKEYSIZE;
        output.seekp((ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex);
        Debug::DelayedPrint::PushToBuffer("Expected File Content key written at: ");
        Debug::DelayedPrint::PushToBufferAndEndLine(std::to_string((ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex).c_str());
        output.write(EncryptedContent.data(), EncryptedContent.size());
        extraIndex += Util::SizeOfString(EncryptedContent);
        Debug::DelayedPrint::PushToBufferAndEndLine("Expected File Content hex: ");
        Debug::DelayedPrint::PushToBufferAndEndLine(Util::toHex(EncryptedContent).c_str());
        Debug::DelayedPrint::PushToBufferAndEndLine("________________________");
    }
    std::string EncryptedFileTable = Util::BuildFilesTable(EncryptedPathsTable, EncryptedFileContent);
    DecryptionAndEncryption::Encrypt(Extended4BytesKey, iv, EncryptedFileTable, EncryptedFileTable);
    DecryptionAndEncryption::Encrypt(_32bytesKey, iv, EncryptedFileTable, EncryptedFileTable);
    output.seekp((ENCRYPTIONKEYSIZE + HEADERSIZE) + Util::SizeOfString(EncryptedPathsTable) + extraIndex);
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