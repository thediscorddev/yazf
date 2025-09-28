#ifndef KEYANDCONTENTPAIR_HPP
#define KEYANDCONTENTPAIR_HPP
#include <string>
namespace YAZF
{
    class KeyAndContentPair
    {
            std::string First, Second;
            std::string Type;
            void CheckSecondType();
        public:
            std::string GetFirst() const;
            std::string GetSecond() const;
            std::string GetContentType() const;
            void AssignFirst(const std::string& first);
            void AssignSecond(const std::string& second);
            KeyAndContentPair(const std::string& first, const std::string& second);
    };
}
#endif