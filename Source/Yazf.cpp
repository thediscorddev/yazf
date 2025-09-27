#include "../Header/Yazf.hpp"
#include <random>
#include <iostream>
#include <stdexcept>
#include "../Header/DecryptionAndEncryption.hpp"
#include "../Header/Util.hpp"
#include <string>
using namespace YAZF;
Yazf::Yazf(const std::string& _64byteskey, const std::string& _32bitkey)
: _64bytesKey(_64byteskey), _32bitKey(_32bitkey) {

}
std::string Yazf::RandomKey(size_t SizeInBytes)
{
    std::string content = "";
    const std::string& predefinedchar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-=`~_+{}[];:'\"\\|,./<>?/*-+";
    // 
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, predefinedchar.size() - 1);

    std::string s;
    content.reserve(SizeInBytes);
    for (std::size_t i = 0; i < SizeInBytes; ++i) {
        content.push_back(predefinedchar[dist(gen)]);
    }
    return content;
}
Yazf Yazf::NewWithRandomEncryptionKey()
{
    return NewWithPredefinedKey(RandomKey(64), RandomKey(4));
}
Yazf Yazf::NewWithPredefinedKey(const std::string& _64byteskey, const std::string _32bitKey)
{
    std::cout << "CreatingKey 64bytes: " <<_64byteskey << std::endl;
    std::cout << "CreatingKey 4bytes: " <<_32bitKey << std::endl;
    std::string _64bytesKey = _64byteskey;
    std::string _32bitkey = _32bitKey;
    if (_64bytesKey.size() < ENCRYPTIONKEYSIZE/8) {
        _64bytesKey.resize(ENCRYPTIONKEYSIZE/8, '\0');
    } else if (_64bytesKey.size() > ENCRYPTIONKEYSIZE/8) {
        _64bytesKey.resize(ENCRYPTIONKEYSIZE/8);
    }
        if (_32bitkey.size() < 4) {
        _32bitkey.resize(4, '\0');
    } else if (_32bitkey.size() > 4) {
        _32bitkey.resize(4);
    }
    return Yazf(_64bytesKey, _32bitKey);
}
void Yazf::Pushfile(const File& file)
{
    FileList.push_back(file);
}
void Yazf::Write(std::filesystem::path Path)
{
    std::ofstream output(Path, std::ios::out | std::ios::binary);
    if(!output.is_open())
    {
        throw std::runtime_error("Failed to open file.");
    }
    // preparing files
    File::GlobalFiles.insert(File::GlobalFiles.end(), FileList.begin(), FileList.end());
    std::string iv(_64bytesKey.begin(), _64bytesKey.begin()+16);
    std::cout << "base iv: " << iv <<std::endl;
    std::string EncryptedPathsTable = "";
    DecryptionAndEncryption::Encrypt(_32bitKey, iv, Util::BuildPathsTable(), EncryptedPathsTable);
    DecryptionAndEncryption::Encrypt(_64bytesKey, iv, EncryptedPathsTable, EncryptedPathsTable);
    // write the first 64 bytes encryption key
    output.seekp(0);
    output.write(_64bytesKey.data(), _64bytesKey.size());
    //write the header next
    //write the encrypted path table next
    output.seekp((ENCRYPTIONKEYSIZE+HEADERSIZE)/8);
    output.write(EncryptedPathsTable.data(), EncryptedPathsTable.size());
    // write the file content next
    // but first, we will encrypt them first
    std::map<std::string, std::pair<std::string, unsigned int>> EncryptedFileContent;
    std::map<std::pair<std::pair<std::string, std::string>, std::string>, std::pair<std::string, unsigned int>> EncryptedFileContentWithSpecialKey;
    for(unsigned int i = 0; i < File::GlobalFiles.size(); i++)
    {
        const auto& file = File::GlobalFiles[i];
        std::string UniqueKey = RandomKey(16);
        std::string EncryptedUniqueKey = UniqueKey;
        // Encrypt the encryption key
        DecryptionAndEncryption::Encrypt(_32bitKey, iv, EncryptedUniqueKey, EncryptedUniqueKey);
        DecryptionAndEncryption::Encrypt(_64bytesKey, iv, EncryptedUniqueKey, EncryptedUniqueKey);
        // we now have the first std::pair<string, string>
        // now we will encrypt the content with the nonencrypted unique key
        std::string Content = file.GetFileContent();
        std::string EncryptedContent = Content;
        DecryptionAndEncryption::Encrypt(UniqueKey, iv, EncryptedContent, EncryptedContent);
        DecryptionAndEncryption::Encrypt(_32bitKey, iv, EncryptedContent, EncryptedContent);
        DecryptionAndEncryption::Encrypt(_64bytesKey, iv, EncryptedContent, EncryptedContent);
        // now we will push element
        std::pair<std::string,std::string> UniqueKeyPair(EncryptedUniqueKey, UniqueKey);
        std::pair<std::pair<std::string,std::string>, std::string> UniqueKeyPairAndContent(UniqueKeyPair, Content);
        std::pair<std::string, unsigned int> EncryptedContentWithIndex(EncryptedContent,i);
        EncryptedFileContentWithSpecialKey[UniqueKeyPairAndContent] = EncryptedContentWithIndex;
        EncryptedFileContent[Content] = EncryptedContentWithIndex;
    }
    size_t extraIndex = 0;
    for(const auto& el: EncryptedFileContentWithSpecialKey)
    {
        const auto& First = el.first;
        const auto& Second = el.second;
        const auto& EncryptedUniqueKey = First.first.first;
        const auto& EncryptedContent = Second.first;
        output.seekp((ENCRYPTIONKEYSIZE+HEADERSIZE)/8+Util::SizeOfString(EncryptedPathsTable)+extraIndex);
        output.write(EncryptedUniqueKey.data(), EncryptedUniqueKey.size());
        extraIndex += 16;
        output.seekp((ENCRYPTIONKEYSIZE+HEADERSIZE)/8+Util::SizeOfString(EncryptedPathsTable)+extraIndex);
        output.write(EncryptedContent.data(), EncryptedContent.size());
        extraIndex += Util::SizeOfString(EncryptedContent);
    }
    std::string EncryptedFileTable = Util::BuildFilesTable(EncryptedPathsTable, EncryptedFileContent);
    DecryptionAndEncryption::Encrypt(_32bitKey, iv, EncryptedFileTable, EncryptedFileTable);
    DecryptionAndEncryption::Encrypt(_64bytesKey, iv, EncryptedFileTable, EncryptedFileTable);
    output.seekp((ENCRYPTIONKEYSIZE+HEADERSIZE)/8+Util::SizeOfString(EncryptedPathsTable)+extraIndex);
    output.write(EncryptedFileTable.data(), EncryptedFileTable.size());
    // we have done writing files table
    // Finally, we will write our header
    // Header will only got encrypted by the 512 bits key
    std::string EncryptedHeader = Util::BuildHeader(_32bitKey, EncryptedPathsTable, EncryptedFileTable, EncryptedFileContent);
    DecryptionAndEncryption::EncryptWithPadding(_64bytesKey, iv, EncryptedHeader, 1024, EncryptedHeader);
    output.seekp(ENCRYPTIONKEYSIZE/8);
    output.write(EncryptedHeader.data(), EncryptedHeader.size());
    output.close();
}
Yazf Yazf::Parse(std::filesystem::path Path)
{
    std::ifstream input(Path, std::ios::binary);
    if (!input) {
        std::cerr << "Failed to open file\n";
        return NewWithRandomEncryptionKey();
    }
    input.seekg(0,std::ios::beg);
    char EncryptionKey512bits[ENCRYPTIONKEYSIZE/8 + 1] = {0};
    input.read(EncryptionKey512bits,ENCRYPTIONKEYSIZE/8);
    std::cout << "Encryption Key: " << EncryptionKey512bits << std::endl;
    std::string iv(EncryptionKey512bits);
    iv.resize(16);
    std::cout << "iv: " << iv << std::endl;
    char EncryptedHeader[HEADERSIZE/8 + 1] = {0};
    input.seekg(ENCRYPTIONKEYSIZE/8,std::ios::beg);
    input.read(EncryptedHeader,HEADERSIZE/8);
    std::string EncryptedHeaderStr(EncryptedHeader);
    DecryptionAndEncryption::Decrypt(EncryptionKey512bits,iv,EncryptedHeader,EncryptedHeaderStr);
    std::cout << "Decrypted Header: "<< EncryptedHeaderStr << std::endl;
    return NewWithRandomEncryptionKey();
}