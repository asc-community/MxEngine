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

namespace MxEngine
{
    Script::Script()
        : scriptImpl(nullptr)
    {
        (void)new(&this->handleImpl)ObjectId();
    }

    Script::Script(const MxString& className)
    {
        (void)new(&this->handleImpl)ObjectId();
        this->SetScriptableObject(className);
    }

    Script::~Script()
    {
        std::launder(reinterpret_cast<ObjectId*>(&this->handleImpl))->~ObjectId();
    }

    void Script::OnUpdate(float dt)
    {
        if (this->scriptImpl != nullptr)
        {
            RuntimeCompiler::UpdateScriptableObject(this->scriptImpl);
        }
    }

    const ObjectId& Script::GetNativeHandle() const
    {
        return *std::launder(reinterpret_cast<const ObjectId*>(&this->handleImpl));
    }

    void Script::SetScriptableObject(const MxString& className)
    {
        ObjectId* id = std::launder(reinterpret_cast<ObjectId*>(&this->handleImpl));
        this->scriptImpl = RuntimeCompiler::CreateScriptableObject(className.c_str(), id);
    }

    void Script::SetScriptableObject(Scriptable* script)
    {
        this->scriptImpl = script;
    }

    Scriptable* Script::GetScriptableObject()
    {
        return this->scriptImpl;
    }

    const Scriptable* Script::GetScriptableObject() const
    {
        return this->scriptImpl;
    }
}