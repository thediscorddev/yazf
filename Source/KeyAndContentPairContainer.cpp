#include "../Header/KeyAndContentPairContainer.hpp"
#include <stdexcept>
using namespace YAZF;
KeyAndContentPairContainer::KeyAndContentPairContainer()
{

}
void KeyAndContentPairContainer::append(const std::string& Key, const std::string& content)
{
    try {
        GetOrThrow(Key,"Key did not exist");
        throw std::runtime_error("Key already exist");
    }catch(const std::runtime_error& error)
    {
        // key not exist
        List.emplace_back(Key,content);
    }
}
void KeyAndContentPairContainer::remove(const std::string& Key)
{
    int i = 0;
    for(i = 0; i < List.size(); i++)
    {
        if(List[i].GetFirst() == Key) break;
    }
    if(i < List.size()) List.erase(List.begin()+i);
}
size_t KeyAndContentPairContainer::size() const
{
    return List.size();
}
KeyAndContentPair KeyAndContentPairContainer::GetOrThrow(const std::string& Key, const std::string& errorMessage) const
{
    for(const auto& el: List)
    {
        if(el.GetFirst() == Key) return el;
    }
    throw std::runtime_error(errorMessage);
}
void KeyAndContentPairContainer::Modify(const std::string& Key, const std::string& Content)
{
    for(auto& el: List)
    {
        if(el.GetFirst() == Key) el.AssignSecond(Content);
    }
    throw std::runtime_error("No key found");
}
std::vector<KeyAndContentPair>& KeyAndContentPairContainer::GetList()
{
    return List;
}