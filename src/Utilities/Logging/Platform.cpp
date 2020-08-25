#include "Platform.h"

#include <iomanip>
#include <ctime>

#include "Core/Macro/Macro.h"

#if defined(MXENGINE_WINDOWS)
#include <Windows.h>
#define BOOST_STACKTRACE_USE_WINDBG_CACHED
#endif

#if defined(MXENGINE_USE_BOOST)
#include <boost/stacktrace.hpp>
#endif
#include <string>

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

    bool starts_with(std::string_view str, std::string_view sub)
    {
        auto strIt = str.begin();
        auto subIt = sub.begin();
        auto strEnd = str.end();
        auto subEnd = sub.end();

        for(; subIt != subEnd; subIt++, strIt++)
        {
            if((strIt == strEnd) || (*strIt != *subIt))
                return false;
        }
        return true;
    }

    void PrintStacktrace(std::ostream& out)
    {
    #if defined(MXENGINE_USE_BOOST)
        auto st = boost::stacktrace::stacktrace().as_vector();
        for (size_t i = 0, size = st.size(); i < size; i++)
        {
            auto function = st[i].name();
            auto filename = st[i].source_file();
            // in C++20 starts_with will be included in standard
            if (!filename.empty() &&
                !starts_with(function, "boost::") &&
                !starts_with(function, "MxEngine::Logger") &&
                !starts_with(function, "std::") &&
                !starts_with(function, "function_call") &&
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
    #endif
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