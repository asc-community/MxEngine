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

#include "Script.h"
#include <RuntimeObjectSystem/IObject.h>
#include "Core/Runtime/RuntimeCompiler.h"
#include "Core/Runtime/Reflection.h"
#include "Core/Components/Scripting/Scriptable.h"

namespace MxEngine
{
    Script::Script(const MxString& className)
    {
        this->SetScriptableObject(className);
    }

    void Script::OnUpdate(float dt)
    {
        if (this->scriptImpl != nullptr)
        {
            RuntimeCompiler::InvokeScriptableObject(
                this->scriptImpl, ScriptableMethod::ON_UPDATE, MxObject::GetByComponent(*this)
            );
        }
    }

    void Script::Init()
    {
        if (this->scriptImpl != nullptr)
        {
            RuntimeCompiler::InvokeScriptableObject(
                this->scriptImpl, ScriptableMethod::ON_CREATE, MxObject::GetByComponent(*this)
            );
        }
    }

    void Script::SetScriptableObject(const MxString& className)
    {
        auto& info = RuntimeCompiler::GetScriptInfo(className);
        this->scriptImpl = info.ScriptHandle;
        this->scriptName = MakeStringId(info.Name);
    }

    void Script::SetScriptableObject(const ScriptInfo& scriptInfo)
    {
        this->scriptImpl = scriptInfo.ScriptHandle;
        this->scriptName = MakeStringId(scriptInfo.Name);
        if (this->scriptImpl != nullptr)
        {
            RuntimeCompiler::InvokeScriptableObject(
                this->scriptImpl, ScriptableMethod::ON_RELOAD, MxObject::GetByComponent(*this)
            );
        }
    }

    StringId Script::GetHashedScriptName() const
    {
        return this->scriptName;
    }

    bool Script::HasScriptableObject() const
    {
        return this->scriptImpl != nullptr;
    }

    void Script::RemoveScriptableObject()
    {
        this->scriptImpl = nullptr;
        this->scriptName = 0;
    }

    const MxString& Script::GetScriptName() const
    {
        return RuntimeCompiler::GetScriptInfo(this->GetHashedScriptName()).Name;
    }

    const MxString& Script::GetScriptFileName() const
    {
        return RuntimeCompiler::GetScriptInfo(this->GetHashedScriptName()).FilePath;
    }

    Scriptable* Script::GetScriptableObject()
    {
        return this->scriptImpl;
    }

    const Scriptable* Script::GetScriptableObject() const
    {
        return this->scriptImpl;
    }

    MXENGINE_REFLECT_TYPE
    {
        using SetScriptName = void(Script::*)(const MxString&);

        rttr::registration::class_<Script>("Script")
            .constructor<>()
            .property_readonly("attached script name", &Script::GetScriptName)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(MetaInfo::CONDITION, +([](const rttr::instance& p) { return p.try_convert<Script>()->HasScriptableObject(); }))
            )
            .property("_name", &Script::GetScriptName, (SetScriptName)&Script::SetScriptableObject)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE),
                rttr::metadata(MetaInfo::CONDITION, +([](const rttr::instance& p) { return p.try_convert<Script>()->HasScriptableObject(); }))
            )
            .property_readonly("attached script filepath", &Script::GetScriptFileName)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(MetaInfo::CONDITION, +([](const rttr::instance& p) { return p.try_convert<Script>()->HasScriptableObject(); }))
            )
            .method("script actions", &Script::GetHashedScriptName)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::CUSTOM_VIEW, GUI::EditorExtra<Script>)
            );
    }
}