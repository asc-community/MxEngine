#include "Logger.h"
#include "LoggerData.h"

#include <iostream>

namespace MxEngine
{
    void Logger::HandleFatalErrors(VerbosityType type)
    {
        if ((type >= VerbosityType::FATAL) && Logger::IsAbortOnFatal())
        {
            Logger::CloseLogFile();
            std::cout.flush();
            AbortApplication();
        }
    }

    void Logger::HandleErrors(VerbosityType type)
    {
        if ((type >= VerbosityType::ERROR) && Logger::IsStacktraceOnError())
        {
            if(Logger::IsLogToConsole())
                PrintStacktrace(std::cout);
            if (Logger::IsLogToFile())
                PrintStacktrace(logger->LogFile);
        }
    }

    const char* Logger::GetVerbosityStringAligned(VerbosityType type)
    {
        switch (type)
        {
        case VerbosityType::DEBUG:
            return "  DEBUG";
        case VerbosityType::INFO:
            return "   INFO";
        case VerbosityType::WARNING:
            return "WARNING";
        case VerbosityType::ERROR:
            return "  ERROR";
        case VerbosityType::FATAL:
            return "  FATAL";
        default:
            return "UNNAMED";
        }
    }

    void Logger::Init()
    {
        logger = new LoggerData();
    }

    LoggerData* Logger::GetImpl()
    {
        return logger;
    }

    void Logger::Clone(LoggerData* data)
    {
        logger = data;
    }

    void Logger::OpenLogFile(const char* filename)
    {
        Logger::CloseLogFile();
        logger->LogFile.open(filename, std::ios::out);
    }

    void Logger::OpenLogFileAppend(const char* filename)
    {
        Logger::CloseLogFile();
        logger->LogFile.open(filename, std::ios::out | std::ios::app);
    }

    void Logger::CloseLogFile()
    {
        logger->LogFile.close();
    }

    bool Logger::IsLogToConsole()
    {
        return logger->LogToConsole;
    }

    bool Logger::IsLogToFile()
    {
        return logger->LogToFile;
    }

    bool Logger::IsLogFileOpened()
    {
        return logger->LogFile.is_open();
    }

    bool Logger::IsAbortOnFatal()
    {
        return logger->AbortOnFatal;
    }

    bool Logger::IsStacktraceOnError()
    {
        return logger->StacktraceOnError;
    }

    void Logger::LogToConsole(const char* text)
    {
        if (Logger::IsLogToConsole())
        {
            std::cout << text;
            std::cout.flush();
        }
    }

    void Logger::LogToFile(const char* text)
    {
        if (Logger::IsLogToFile())
        {
            logger->LogFile << text;
            logger->LogFile.flush();
        }
    }

    void Logger::LogLineToConsole(const char* text)
    {
        if (Logger::IsLogToConsole())
        {
            std::cout << text << '\n';
            std::cout.flush();
        }
    }

    void Logger::LogLineToFile(const char* text)
    {
        if (Logger::IsLogToFile())
        {
            logger->LogFile << text << '\n';
            logger->LogFile.flush();
        }
    }

    void Logger::Log(VerbosityType type, const char* text)
    {
        if ((uint8_t)type >= (uint8_t)Logger::GetVerbosityLevel())
        {
            SetConsoleColor(logger->Colors[(size_t)type]);
            Logger::LogLineToConsole(text);
            SetConsoleColor(ConsoleColor::GRAY);

            Logger::LogToFile(Logger::GetVerbosityStringAligned(type));
            Logger::LogToFile(" > ");
            Logger::LogLineToFile(text);

            Logger::HandleErrors(type);
            Logger::HandleFatalErrors(type);
        }
    }

    void Logger::Log(VerbosityType type, const MxString& caller, const MxString& message)
    {
        auto text = '[' + GetCurrentTime() + ' ' + caller + "]: " + message;
        Logger::Log(type, text.c_str());
    }

    void Logger::SetAbortOnFatal(bool value)
    {
        logger->AbortOnFatal = value;
    }

    void Logger::SetStacktraceOnError(bool value)
    {
        logger->StacktraceOnError = value;
    }

    void Logger::SetLogConsole(bool value)
    {
        logger->LogToConsole = value;
    }

    void Logger::SetLogFile(bool value)
    {
        logger->LogToFile = value;
    }

    void Logger::SetLogLevel(VerbosityLevel level)
    {
        logger->Verbosity = level;
    }

    void Logger::SetLogColor(VerbosityType type, ConsoleColor color)
    {
        logger->Colors[(size_t)type] = color;
    }

    VerbosityLevel Logger::GetVerbosityLevel()
    {
        return logger->Verbosity;
    }
}