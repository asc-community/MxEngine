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

// forward-declarations of RCC++ interfaces
struct ICompilerLogger;
struct IObjectConstructor;
class RuntimeObjectSystem;
struct ObjectId;

#include "Utilities/STL/MxVector.h"
#include "Utilities/String/String.h"
#include "Utilities/STL/MxString.h"
#include "Utilities/STL/MxHashMap.h"

namespace MxEngine
{
    class UpdateListener;
    class Scriptable;
    class MxObject;

    enum class ScriptableMethod
    {
        ON_CREATE,
        ON_RELOAD,
        ON_UPDATE,
    };

    struct ScriptInfo
    {
        using ScriptId = std::aligned_storage_t<16>;

        MxString FileName;
        MxString Name;
        Scriptable* ScriptHandle = nullptr;
        ScriptId ScriptHandleId{ };
    };

    struct RuntimeCompilerImpl
    {
        UpdateListener* updateListener = nullptr;
        ICompilerLogger* compilerLogger = nullptr;
        RuntimeObjectSystem* runtimeObjectSystem = nullptr;
        MxHashMap<StringId, ScriptInfo> registeredScripts;
    };

    class RuntimeCompiler
    {
        inline static RuntimeCompilerImpl* impl = nullptr;

        static void RegisterExistingScripts();
        static void RegisterNewScript(IObjectConstructor* constructor);
    public:
        static void Init();
        static void Clone(RuntimeCompilerImpl* other);
        static RuntimeCompilerImpl* GetImpl();

        static bool HasNewCompiledModules();
        static bool HasCompilationTaskInProcess();
        static void LoadCompiledModules();
        static void OnUpdate(float dt);

        static const ScriptInfo& GetScriptInfo(const MxString& scriptName);
        static const ScriptInfo& GetScriptInfo(StringId scriptName);
        static void InvokeScriptableObject(Scriptable* script, ScriptableMethod method, MxObject& scriptParent);
        static void AddScriptFile(const MxString& scriptName, const MxString& scriptFileName);
        static const MxHashMap<StringId, ScriptInfo>& GetRegisteredScripts();
        static void UpdateScriptableObject(const MxString& scriptName, Scriptable* script);
    };
}