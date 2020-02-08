#pragma once

#include "Utilities/Time/Time.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"

#include <fstream>
#include <string>

namespace MxEngine
{
    class ProfileSession
    {
        std::ofstream stream;
        size_t count = 0;

        void WriteJsonHeader();
        void WriteJsonFooter();
    public:
        bool IsValid() const;
        size_t GetEntryCount() const;
        void StartSession(const std::string& filename);
        void WriteJsonEntry(const char* function, TimeStep begin, TimeStep delta);
        void EndSession();
    };

    using Profiler = SingletonHolder<ProfileSession>;

    class ScopeProfiler
    {
        TimeStep start;
        const char* function;
        ProfileSession& profiler;
    public:
        inline ScopeProfiler(ProfileSession& profiler, const char* function)
            : start(Time::Current()), function(function), profiler(profiler) { }

        inline ~ScopeProfiler()
        {
            TimeStep end = Time::Current();
            this->profiler.WriteJsonEntry(this->function, this->start, end - start);
        }
    };

    class ScopeTimer
    {
        TimeStep start;
        std::string function;
        std::string_view invoker;
    public:
        inline ScopeTimer(std::string_view invoker, std::string_view function)
            : start(Time::Current()), invoker(invoker), function(function)
        {
            Logger::Instance().Debug(this->invoker, "calling " + this->function);
        }

        inline ~ScopeTimer()
        {
            TimeStep end = Time::Current();
            std::string delta = BeautifyTime(end - start);
            Logger::Instance().Debug(this->invoker, this->function + " finished in " + delta);
        }
    };

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define MAKE_SCOPE_PROFILER(function) ScopeProfiler CONCAT(_profiler, __LINE__)(Profiler::Instance(), function)
#define MAKE_SCOPE_TIMER(invoker, function) ScopeTimer CONCAT(_timer, __LINE__)(invoker, function)
}