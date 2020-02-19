// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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