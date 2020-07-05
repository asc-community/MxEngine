#include "Platform.h"

#include <iomanip>
#include <ctime>

#include "Core/Macro/Macro.h"
#include <boost/stacktrace.hpp>


#if defined(MXENGINE_WINDOWS)
#include <Windows.h>
#endif

namespace MxEngine
{
    void SetConsoleColor(ConsoleColor color)
    {
        #if defined(MXENGINE_WINDOWS)
        constexpr WORD ColorTable[] =
        {
            0,
            FOREGROUND_BLUE,
            FOREGROUND_GREEN,
            FOREGROUND_GREEN | FOREGROUND_BLUE,
            FOREGROUND_RED,
            FOREGROUND_RED | FOREGROUND_BLUE,
            FOREGROUND_RED | FOREGROUND_GREEN,
            FOREGROUND_INTENSITY,
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_GREEN,
            FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_RED,
            FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
            FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
        };

        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(h, ColorTable[(size_t)color]);
        #endif
    }

    void PrintStacktrace(std::ostream& out)
    {
        auto st = boost::stacktrace::stacktrace().as_vector();
        for (size_t i = 0, size = st.size(); i < size; i++)
        {
            auto function = st[i].name();
            auto filename = st[i].source_file();
            // in C++20 starts_with will replace _Starts_with
            if (!filename.empty() &&
                !function._Starts_with("boost::") &&
                !function._Starts_with("MxEngine::Logger") &&
                !function._Starts_with("std::") &&
                !function._Starts_with("function_call") &&
                function.find("main") == function.npos &&
                function.find("PrintStacktrace") == function.npos &&
                function.find("lambda_") == function.npos
                )
            {
                out << "  at " << function;
                out << " in " << filename << ':' << st[i].source_line();
                out << " | " << st[i].address() << '\n';
            }
        }
    }

    #undef GetCurrentTime // win api
    MxString GetCurrentTime()
    {
        auto t = std::time(nullptr);
        #pragma warning(suppress : 4996)
        auto tm = *std::localtime(&t);

        char buffer[16];
        std::strftime(buffer, std::size(buffer), "%H:%M:%S", &tm); 
        return buffer;
    }


    void AbortApplication()
    {
        #if defined(MXENGINE_WINDOWS) && !defined(MXENGINE_SHIPPING)
            DebugBreak();
        #else
            std::abort();
        #endif
    }
}