#ifndef KEYANDCONTENTPAIRCONTAINER_HPP
#define KEYANDCONTENTPAIRCONTAINER_HPP
#include <vector>
#include <string>
#include "KeyAndContentPair.hpp"
namespace YAZF
{
    class KeyAndContentPairContainer
    {
            std::vector<KeyAndContentPair> List;
        public:
            KeyAndContentPairContainer();
            void append(const std::string& Key, const std::string& content);
            void remove(const std::string& Key);
            size_t size() const;
            KeyAndContentPair GetOrThrow(const std::string& Key, const std::string& errorMessage) const;
            void Modify(const std::string& Key, const std::string& Content);
            std::vector<KeyAndContentPair>& GetList();
    };
}
#endif