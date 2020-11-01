#pragma once

#include "LogSettings.h"
#include "Platform.h"
#include "Utilities/STL/MxString.h"

namespace MxEngine
{
    struct LoggerData;

    class Logger
    {
        inline static LoggerData* logger = nullptr;

        static void HandleFatalErrors(VerbosityType type);
        static void HandleErrors(VerbosityType type);
        static const char* GetVerbosityStringAligned(VerbosityType type);
    public:
        static void Init();
        static LoggerData* GetImpl();
        static void Clone(LoggerData* data);

        static void LogToConsole(const char* text);
        static void LogToFile(const char* text);
        static void LogLineToConsole(const char* text);
        static void LogLineToFile(const char* text);

        static void Log(VerbosityType type, const char* text);
        static void Log(VerbosityType type, const MxString& caller, const MxString& message);

        static void OpenLogFile(const char* filename);
        static void OpenLogFileAppend(const char* filename);
        static void CloseLogFile();

        static bool IsLogToConsole();
        static bool IsLogToFile();
        static bool IsLogFileOpened();
        static bool IsAbortOnFatal();
        static bool IsStacktraceOnError();

        static void SetAbortOnFatal(bool value);
        static void SetStacktraceOnError(bool value);
        static void SetLogConsole(bool value);
        static void SetLogFile(bool value);
        static void SetLogLevel(VerbosityLevel level);
        static void SetLogColor(VerbosityType type, ConsoleColor color);

        static VerbosityLevel GetVerbosityLevel();
    };

    #if defined(MXENGINE_SHIPPING)
        #define MXLOG_DEBUG(caller, ...)  
        #define MXLOG_INFO(caller, ...)   
        #define MXLOG_WARNING(caller, ...)
        #define MXLOG_ERROR(caller, ...)  
        #define MXLOG_FATAL(caller, ...) MxEngine::AbortApplication()
    #else
        #if defined(MXENGINE_RELEASE)
            #define MXLOG_DEBUG(caller, ...)
        #else
            #define MXLOG_DEBUG(caller, ...) MxEngine::Logger::Log(MxEngine::VerbosityType::DEBUG, caller, __VA_ARGS__)
        #endif

        #define MXLOG_INFO(caller, ...)    MxEngine::Logger::Log(MxEngine::VerbosityType::INFO,    caller, __VA_ARGS__)
        #define MXLOG_WARNING(caller, ...) MxEngine::Logger::Log(MxEngine::VerbosityType::WARNING, caller, __VA_ARGS__)
        #define MXLOG_ERROR(caller, ...)   MxEngine::Logger::Log(MxEngine::VerbosityType::ERROR,   caller, __VA_ARGS__)
        #define MXLOG_FATAL(caller, ...)   MxEngine::Logger::Log(MxEngine::VerbosityType::FATAL,   caller, __VA_ARGS__)
    #endif
}