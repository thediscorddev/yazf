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
std::string Yazf::RandomUniqueKey(size_t SizeInBytes)
{
    std::string content = "";
    const std::string &predefinedchar = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_+=";
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