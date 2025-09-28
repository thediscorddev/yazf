#include "../Header/KeyAndContentPair.hpp"
#include "../Header/Util.hpp"
using namespace YAZF;
std::string KeyAndContentPair::GetFirst() const
{
    return First;
}
std::string KeyAndContentPair::GetSecond() const
{
    return Second;
}
std::string KeyAndContentPair::GetContentType() const
{
    return Type;
}
void KeyAndContentPair::AssignFirst(const std::string &first)
{
    First = first;
}
void KeyAndContentPair::AssignSecond(const std::string &second)
{
    Second = second;
    CheckSecondType();
}
KeyAndContentPair::KeyAndContentPair(const std::string &first, const std::string &second)
    : First(first), Second(second)
{
    CheckSecondType();
}
void KeyAndContentPair::CheckSecondType()
{
    if (Util::isInt(Second))
    {
        Type = "int";
    }
    else if (Util::isFloat(Second))
    {
        Type = "float";
    }
    else if (Util::isDouble(Second))
    {
        Type = "double";
    }
    else
    {
        Type = "string";
    }
}