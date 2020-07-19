// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
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

#include "Core/Application/Application.h"

namespace MxEngine
{
    class Runtime
    {
        // callback must be in format `MxString Func(EventType& e)`
        template<typename R, typename T> static T DeduceEventTypeFromCallback(std::function<R(T&)>) { return std::declval<T>(); };
    public:
        template<typename Func>
        static void RegisterComponentEditor(const char* name, Func&& callback)
        {
            Application::Get()->GetRuntimeEditor().RegisterComponentEditor(name, std::forward<Func>(callback));
        }

        template<typename T>
        static void RegisterComponentUpdate()
        {
            Application::Get()->RegisterComponentUpdate<T>();
        }

        template<typename Func>
        static void RegisterEventLogger(Func&& callback)
        {
            using EventType = decltype(Runtime::DeduceEventTypeFromCallback(std::function{ std::declval<Func>() }));
            static_assert(std::is_convertible_v<std::invoke_result_t<Func, EventType&>, MxString>, "callback must return value convertable to MxString");
            Application::Get()->GetEventDispatcher().AddEventListener("EventLogger", [f = std::forward<Func>(callback)](EventType& e) { Runtime::AddEventLogEntry(f(e)); });
        }

        static void AddEventLogEntry(const MxString& entry)
        {
            Application::Get()->GetRuntimeEditor().AddEventEntry(entry);
        }

        static bool IsEditorActive()
        {
            return Application::Get()->GetRuntimeEditor().IsActive();
        }

        static void ExecuteScript(const MxString& script)
        {
            Application::Get()->GetRuntimeEditor().ExecuteScript(script);
        }

        static void CloseApplication()
        {
            Application::Get()->CloseApplication();
        }
    };
}