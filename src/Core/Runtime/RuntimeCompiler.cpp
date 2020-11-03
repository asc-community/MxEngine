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

#include "RuntimeCompiler.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Components/Scripting/Script.h"
#include "Core/Components/Scripting/Scriptable.h"

#include <RuntimeObjectSystem/RuntimeObjectSystem.h>
#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeCompiler/ICompilerLogger.h>

#if defined MXENGINE_WINDOWS
    #define MXENGINE_RCCPP_STANDARD_COMPILER_OPTION "/std:c++17"
#else
    #define MXENGINE_RCCPP_STANDARD_COMPILER_OPTION "-std=c++17"
#endif

// WinAPI
#undef ERROR

namespace MxEngine
{
    std::array LibraryNames = {
        #if defined(MXENGINE_LIB_NAME0)
        MXENGINE_LIB_NAME0,
        #endif
        #if defined(MXENGINE_LIB_NAME1)
        MXENGINE_LIB_NAME1,
        #endif
        #if defined(MXENGINE_LIB_NAME2)
        MXENGINE_LIB_NAME2,
        #endif
        #if defined(MXENGINE_LIB_NAME3)
        MXENGINE_LIB_NAME3,
        #endif
        #if defined(MXENGINE_LIB_NAME4)
        MXENGINE_LIB_NAME4,
        #endif
        #if defined(MXENGINE_LIB_NAME5)
        MXENGINE_LIB_NAME5,
        #endif
        #if defined(MXENGINE_LIB_NAME6)
        MXENGINE_LIB_NAME6,
        #endif
        #if defined(MXENGINE_LIB_NAME7)
        MXENGINE_LIB_NAME7,
        #endif
        #if defined(MXENGINE_LIB_NAME8)
        MXENGINE_LIB_NAME8,
        #endif
        #if defined(MXENGINE_LIB_NAME9)
        MXENGINE_LIB_NAME9,
        #endif
        #if defined(MXENGINE_LIB_NAME10)
        MXENGINE_LIB_NAME10,
        #endif
        #if defined(MXENGINE_LIB_NAME11)
        MXENGINE_LIB_NAME11,
        #endif
        #if defined(MXENGINE_LIB_NAME12)
        MXENGINE_LIB_NAME12,
        #endif
        #if defined(MXENGINE_LIB_NAME13)
        MXENGINE_LIB_NAME13,
        #endif
        #if defined(MXENGINE_LIB_NAME14)
        MXENGINE_LIB_NAME14,
        #endif
        #if defined(MXENGINE_LIB_NAME15)
        MXENGINE_LIB_NAME15,
        #endif
        #if defined(MXENGINE_LIB_NAME16)
        MXENGINE_LIB_NAME16,
        #endif
        #if defined(MXENGINE_LIB_NAME17)
        MXENGINE_LIB_NAME17,
        #endif
        #if defined(MXENGINE_LIB_NAME18)
        MXENGINE_LIB_NAME18,
        #endif
        #if defined(MXENGINE_LIB_NAME19)
        MXENGINE_LIB_NAME19,
        #endif
        #if defined(MXENGINE_LIB_NAME20)
        MXENGINE_LIB_NAME20,
        #endif
    };

    std::array LibraryDirectories = {
        #if defined(MXENGINE_LIB_DIR0)
        MXENGINE_LIB_DIR0,
        #endif
        #if defined(MXENGINE_LIB_DIR1)
        MXENGINE_LIB_DIR1,
        #endif
        #if defined(MXENGINE_LIB_DIR2)
        MXENGINE_LIB_DIR2,
        #endif
        #if defined(MXENGINE_LIB_DIR3)
        MXENGINE_LIB_DIR3,
        #endif
        #if defined(MXENGINE_LIB_DIR4)
        MXENGINE_LIB_DIR4,
        #endif
        #if defined(MXENGINE_LIB_DIR5)
        MXENGINE_LIB_DIR5,
        #endif
        #if defined(MXENGINE_LIB_DIR6)
        MXENGINE_LIB_DIR6,
        #endif
        #if defined(MXENGINE_LIB_DIR7)
        MXENGINE_LIB_DIR7,
        #endif
        #if defined(MXENGINE_LIB_DIR8)
        MXENGINE_LIB_DIR8,
        #endif
        #if defined(MXENGINE_LIB_DIR9)
        MXENGINE_LIB_DIR9,
        #endif
        #if defined(MXENGINE_LIB_DIR10)
        MXENGINE_LIB_DIR10,
        #endif
        #if defined(MXENGINE_LIB_DIR11)
        MXENGINE_LIB_DIR11,
        #endif
        #if defined(MXENGINE_LIB_DIR12)
        MXENGINE_LIB_DIR12,
        #endif
        #if defined(MXENGINE_LIB_DIR13)
        MXENGINE_LIB_DIR13,
        #endif
        #if defined(MXENGINE_LIB_DIR14)
        MXENGINE_LIB_DIR14,
        #endif
        #if defined(MXENGINE_LIB_DIR15)
        MXENGINE_LIB_DIR15,
        #endif
        #if defined(MXENGINE_LIB_DIR16)
        MXENGINE_LIB_DIR16,
        #endif
        #if defined(MXENGINE_LIB_DIR17)
        MXENGINE_LIB_DIR17,
        #endif
        #if defined(MXENGINE_LIB_DIR18)
        MXENGINE_LIB_DIR18,
        #endif
        #if defined(MXENGINE_LIB_DIR19)
        MXENGINE_LIB_DIR19,
        #endif
        #if defined(MXENGINE_LIB_DIR20)
        MXENGINE_LIB_DIR20,
        #endif
    };

    static_assert(AssertEquality<LibraryNames.size(), LibraryDirectories.size()>::value,
        "not all libraries specified for runtime build");
    static_assert(AssertEquality<LibraryNames.size(), MXENGINE_RCCPP_LIBRARY_COUNT>::value,
        "not all libraries specified for runtime build");

    auto SplitString(const MxString& str, char sep)
    {
        MxVector<MxString> result;
        std::pair<size_t, size_t> indecies(0, 0);
        while ((indecies.second = str.find(sep, indecies.first)) != str.npos)
        {
            result.push_back(str.substr(indecies.first, indecies.second - indecies.first));
            indecies.first = indecies.second + 1;
        }
        if (indecies.first < str.size()) 
            result.push_back(str.substr(indecies.first, str.size() - indecies.first));
        return result;
    }

    auto GetIncludeDirectories()
    {
        return SplitString(MXENGINE_RCCPP_INCLUDE_DIRS, ',');
    }

    auto GetLibraryDirectories()
    {
        MxVector<MxString> result;
        for (const auto& directory : LibraryDirectories)
        {
            result.push_back(directory);
        }
        return result;
    }

    auto GetStandardCompilerOption()
    {
        MxString standard = MXENGINE_RCCPP_STANDARD_COMPILER_OPTION;
        return standard;
    }

    auto GetWorkingDirectory()
    {
        MxString directory = ToMxString(FileManager::GetWorkingDirectory());
        std::transform(directory.begin(), directory.end(), directory.begin(), 
            [](char c) { return c == '\\' ? '/' : c; });
        return directory;
    }

    auto GetLibraryNames()
    {
        MxVector<MxString> result;
        for (const auto& libraryName : LibraryNames)
        {
            auto& lib = result.emplace_back(libraryName);
            size_t extenstionIndex = lib.find(".dll");
            if (extenstionIndex != lib.npos)
            {
                lib[extenstionIndex + 1] = 'l';
                lib[extenstionIndex + 2] = 'i';
                lib[extenstionIndex + 3] = 'b';
            }
        }
        // TODO: try building this code on other platforms
        #if defined(MXENGINE_WINDOWS) // extra Windows-specific libraries
        result.insert(result.end(), {
            "opengl32.lib", "kernel32.lib", "user32.lib", "gdi32.lib",
            "winspool.lib", "comdlg32.lib", "advapi32.lib", "shell32.lib",
            "ole32.lib", "oleaut32.lib", "uuid.lib", "odbc32.lib", "odbccp32.lib"
        });
        #endif
        return result;
    }

    class CompilerLogger : public ICompilerLogger
    {
    public:
        virtual void LogError(const char* format, ...) override
        {
            constexpr size_t bufferSize = 512;
            char buffer[bufferSize + 1] = { '\0' };
            va_list argptr;
            va_start(argptr, format);
            int symbols = vsnprintf(buffer, bufferSize, format, argptr);
            va_end(argptr);
            if (symbols > 0 && symbols <= bufferSize) buffer[symbols - 1] = '\0';

            MXLOG_ERROR("MxEngine::RuntimeCompiler", buffer);
        }

        virtual void LogWarning(const char* format, ...) override
        {
            constexpr size_t bufferSize = 512;
            char buffer[bufferSize + 1] = { '\0' };
            va_list argptr;
            va_start(argptr, format);
            int symbols = vsnprintf(buffer, bufferSize, format, argptr);
            va_end(argptr);
            if (symbols > 0 && symbols <= bufferSize) buffer[symbols - 1] = '\0';

            MXLOG_WARNING("MxEngine::RuntimeCompiler", buffer);
        }

        virtual void LogInfo(const char* format, ...) override
        {
            constexpr size_t bufferSize = 512;
            char buffer[bufferSize + 1] = { '\0' };
            va_list argptr;
            va_start(argptr, format);
            int symbols = vsnprintf(buffer, bufferSize, format, argptr);
            va_end(argptr);
            if (symbols > 0 && symbols <= bufferSize) buffer[symbols - 1] = '\0';

            MXLOG_DEBUG("MxEngine::RuntimeCompiler", buffer);
        }
    };

    static_assert(AssertEquality<sizeof(ObjectId), sizeof(ScriptInfo::ScriptHandleId)>::value,
        "object storage must fit type size");

    class UpdateListener : public IObjectFactoryListener
    {
    public:
        virtual void OnConstructorsAdded() override
        {
            MAKE_SCOPE_PROFILER("RuntimeCompiler::OnNewModuleCompiled");
            MAKE_SCOPE_TIMER("MxEngine::RuntimeCompiler", "RuntimeCompiler::OnNewModuleCompiled");

            auto view = ComponentFactory::GetView<Script>();
            for (auto& script : view)
            {
                auto scriptableObject = script.GetScriptableObject();
                if (scriptableObject != nullptr)
                {
                    auto scriptName = script.GetHashedScriptName();
                    auto& info = RuntimeCompiler::GetScriptInfo(scriptName);
                    auto& id = *std::launder(reinterpret_cast<const ObjectId*>(&info.ScriptHandleId));
                    IObject* newObject = RuntimeCompiler::GetImpl()->runtimeObjectSystem->GetObjectFactorySystem()->GetObject(id);

                    if (newObject != nullptr)
                    {
                        Scriptable* newScript = nullptr;
                        newObject->GetInterface(&newScript);
                        if (newScript != nullptr)
                        {
                            if (newScript != info.ScriptHandle)
                                RuntimeCompiler::UpdateScriptableObject(info.Name, newScript);

                            auto context = GlobalContextSerializer::Serialize();
                            newScript->InitializeModuleContext(reinterpret_cast<void*>(&context));
                            script.SetScriptableObject(info);
                        }
                    }
                }
            }
        }

    };

    void RuntimeCompiler::RegisterExistingScripts()
    {
        AUDynArray<IObjectConstructor*> constructors;
        RuntimeCompiler::GetImpl()->runtimeObjectSystem->GetObjectFactorySystem()->GetAll(constructors);
        for (size_t i = 0; i < constructors.Size(); i++)
        {
            auto& constructor = constructors[i];
            const char* filename = constructor->GetFileName();
            const char* name = constructor->GetName();
            if (name != nullptr)
            {
                ScriptInfo info;
                info.Name = name;
                info.FileName = (filename == nullptr ? "" : filename);
                auto hash = MakeStringId(info.Name);

                auto object = constructor->Construct();
                if (object == nullptr)
                {
                    MXLOG_ERROR("MxEngine::RuntimeCompiler", "cannot create Scriptable object: object construction failed");
                    continue;
                }
                object->GetInterface(&info.ScriptHandle);
                if (info.ScriptHandle == nullptr)
                {
                    MXLOG_ERROR("MxEngine::RuntimeCompiler", "cannot create Scriptable object: object has not Scriptable interface");
                    continue;
                }
                if (impl->registeredScripts.find(hash) != impl->registeredScripts.end())
                {
                    MXLOG_ERROR("MxEngine::RuntimeCompiler", "hash collision or script name dublicate: " + info.Name);
                    continue;
                }

                auto context = GlobalContextSerializer::Serialize();
                info.ScriptHandle->InitializeModuleContext(reinterpret_cast<void*>(&context));
                (void) new(&info.ScriptHandleId) ObjectId(object->GetObjectId());
                impl->registeredScripts[hash] = std::move(info);
            }
        }
    }

    void RuntimeCompiler::Init()
    {
        impl = new RuntimeCompilerImpl();
        impl->updateListener = new UpdateListener();
        impl->compilerLogger = new CompilerLogger();
        impl->runtimeObjectSystem = new RuntimeObjectSystem();
        impl->runtimeObjectSystem->Initialise(impl->compilerLogger, nullptr);
        impl->runtimeObjectSystem->GetObjectFactorySystem()->AddListener(impl->updateListener);
        impl->runtimeObjectSystem->SetAdditionalCompileOptions(GetStandardCompilerOption().c_str());
        impl->runtimeObjectSystem->AddIncludeDir(GetWorkingDirectory().c_str());

        auto includeDirectories = GetIncludeDirectories();
        for (const auto& includeDirectory : includeDirectories)
        {
            impl->runtimeObjectSystem->AddIncludeDir(includeDirectory.c_str());
        }

        auto libraryDirectories = GetLibraryDirectories();
        for (const auto& libraryDirectory : libraryDirectories)
        {
            impl->runtimeObjectSystem->AddLibraryDir(libraryDirectory.c_str());
        }

        auto libraryNames = GetLibraryNames();
        MxString linkOptions;
        for (const auto& libraryName : libraryNames)
        {
            #if defined(MXENGINE_WINDOWS)
            linkOptions += ' ';
            #elif defined(MXENGINE_LINUX)
            linkOptions += " -";
            #else
            linkOptions += " -framework ";
            #endif
            linkOptions += libraryName;
        }
        impl->runtimeObjectSystem->SetAdditionalLinkOptions(linkOptions.c_str());

        RuntimeCompiler::RegisterExistingScripts();
    }

    void RuntimeCompiler::Clone(RuntimeCompilerImpl* other)
    {
        impl = other;
    }

    const MxHashMap<StringId, ScriptInfo>& RuntimeCompiler::GetRegisteredScripts()
    {
        return impl->registeredScripts;
    }

    void RuntimeCompiler::UpdateScriptableObject(const MxString& scriptName, Scriptable* script)
    {
        impl->registeredScripts[MakeStringId(scriptName)].ScriptHandle = script;
    }

    RuntimeCompilerImpl* RuntimeCompiler::GetImpl()
    {
        return impl;
    }

    bool RuntimeCompiler::HasNewCompiledModules()
    {
        return impl->runtimeObjectSystem->GetIsCompiledComplete();
    }

    bool RuntimeCompiler::HasCompilationTaskInProcess()
    {
        return impl->runtimeObjectSystem->GetIsCompiling();
    }

    void RuntimeCompiler::LoadCompiledModules()
    {
        MAKE_SCOPE_PROFILER("RuntimeCompiler::LoadCompiledModules");
        MAKE_SCOPE_TIMER("MxEngine::RuntimeCompiler", "RuntimeCompiler::LoadCompiledModules");
        impl->runtimeObjectSystem->LoadCompiledModule();
    }

    void RuntimeCompiler::OnUpdate(float dt)
    {
        if (RuntimeCompiler::HasNewCompiledModules())
        {
            RuntimeCompiler::LoadCompiledModules();
        }

        MAKE_SCOPE_PROFILER("RuntimeCompiler::OnUpdate()");
        impl->runtimeObjectSystem->GetFileChangeNotifier()->Update(dt);
    }

    const ScriptInfo& RuntimeCompiler::GetScriptInfo(const MxString& scriptName)
    {
        const ScriptInfo& info = RuntimeCompiler::GetScriptInfo(MakeStringId(scriptName));
        if (info.ScriptHandle == nullptr)
        {
            MXLOG_ERROR("MxEngine::RuntimeCompiler", "cannot find script with name: " + scriptName);
        }
        return info;
    }

    const ScriptInfo& RuntimeCompiler::GetScriptInfo(StringId scriptName)
    {
        auto cacheEntry = impl->registeredScripts.find(scriptName);
        if (cacheEntry == impl->registeredScripts.end())
        {
            static ScriptInfo defaultScriptInfo;
            return defaultScriptInfo;
        }
        return cacheEntry->second;
    }

    void RuntimeCompiler::InvokeScriptableObject(Scriptable* script, ScriptableMethod method, MxObject& scriptParent)
    {
        script->CurrentState.Method = method;
        script->CurrentState.Self = std::addressof(scriptParent);
        impl->runtimeObjectSystem->TryProtectedFunction(script);
    }
}