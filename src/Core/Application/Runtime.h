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
#include "Core/Runtime/RuntimeEditor.h"
#include "Core/Runtime/RuntimeCompiler.h"
#include "Utilities/EventDispatcher/EventDispatcher.h"

namespace MxEngine 
{
    class Runtime
    {
    public:
        template<typename Func>
        static void RegisterComponentEditor(const char* name, Func&& callback)
        {
            Application::GetImpl()->GetRuntimeEditor().RegisterComponentEditor(name, std::forward<Func>(callback));
        }

        template<typename T>
        static void RegisterComponentUpdate()
        {
            Application::GetImpl()->RegisterComponentUpdate<T>();
        }

        template<typename EventType, typename Func>
        static void RegisterEventLogger(Func&& callback)
        {
            Application::GetImpl()->GetEventDispatcher().AddEventListener<EventType>(
                "EventLogger",
                [f = std::forward<Func>(callback)](EventType& e)
            {
                Runtime::AddEventLogEntry(f(e));
            }
            );
        }

        static void AddShaderUpdateListener(const ShaderHandle& shader)
        {
            Application::GetImpl()->GetRuntimeEditor().AddShaderUpdateListener<ShaderHandle>(shader);
        }

        static const MxHashMap<StringId, ScriptInfo>& GetRegisteredScripts()
        {
            return RuntimeCompiler::GetRegisteredScripts();
        }

        static bool HasCompilationTaskInProcess()
        {
            return RuntimeCompiler::HasCompilationTaskInProcess();
        }

        static void AddShaderUpdateListener(const ShaderHandle& shader, const FilePath& lookupDirectory)
        {
            Application::GetImpl()->GetRuntimeEditor().AddShaderUpdateListener<ShaderHandle, FilePath>(shader, lookupDirectory);
        }

        static void AddEventLogEntry(const MxString &entry)
        {
            Application::GetImpl()->GetRuntimeEditor().AddEventEntry(entry);
        }

        static bool IsEditorActive()
        {
            return Application::GetImpl()->GetRuntimeEditor().IsActive();
        }

        static void CloseApplication() 
        {
            Application::GetImpl()->CloseApplication();
        }
    };
}