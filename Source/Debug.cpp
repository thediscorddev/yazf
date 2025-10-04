#include "../Header/Debug.hpp"
#include <iostream>
#include <cstdlib>
using namespace YAZF;
std::vector<const char*> Debug::DelayedPrint::PrintList;
void Debug::DelayedPrint::PushToBuffer(const char* Content){
    PrintList.push_back(Content);
}
void Debug::DelayedPrint::PushToBufferAndEndLine(const char* Content){
    PrintList.push_back(Content);
    PrintList.push_back("\n");
}
void Debug::DelayedPrint::Endline(){
    PrintList.push_back("\n");
}
void Debug::DelayedPrint::PrintBuffer(){
    system("clear");
    for(const auto& el: PrintList)
    {
        std::cout << el;
    }
}
void Debug::DelayedPrint::FlushBuffer(){
    PrintList.clear();
}
void Debug::DelayedPrint::PrintAndFlushBuffer(){
    PrintBuffer();
    FlushBuffer();
}
void Debug::DelayedPrint::PrintAndFlushBuffer(const char* Content){
    PrintList.push_back(Content);
    for(const auto& el: PrintList)
    {
        std::cout << el;
    }
    FlushBuffer();
}