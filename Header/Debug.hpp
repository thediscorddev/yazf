#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <string>
#include <vector>
namespace YAZF
{
    class Debug
    {
        public:
        class DelayedPrint {
            static std::vector<const char*> PrintList;
            public:
                static void PushToBuffer(const char* Content);
                static void PushToBufferAndEndLine(const char* Content);
                static void Endline();
                static void PrintBuffer();
                static void FlushBuffer();
                static void PrintAndFlushBuffer();
                static void PrintAndFlushBuffer(const char* Content);
        };
    };
}
#endif