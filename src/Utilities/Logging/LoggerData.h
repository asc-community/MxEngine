#pragma once

#include <fstream>

#include "LogSettings.h"
#include "Platform.h"

namespace MxEngine
{
    struct LoggerData
    {
        std::ofstream LogFile;

        VerbosityLevel Verbosity = VerbosityLevel::ALL;
        bool AbortOnFatal = true;
        bool StacktraceOnError = true;
        bool LogToConsole = true;
        bool LogToFile = false;

        ConsoleColor Colors[5] =
        {
            ConsoleColor::DARK_GRAY, // debug
            ConsoleColor::WHITE, // info
            ConsoleColor::YELLOW, // warning
            ConsoleColor::RED, // error
            ConsoleColor::DARK_RED, // fatal
        };
    };
}