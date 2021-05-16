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

#include "MxObject.h"

namespace MxEngine
{
    MxObject::Handle MxObject::Create()
    {
        auto object = Factory<MxObject>::Create();
        object->handle = object.GetHandle();
        object.MakeStatic();
        return object;
    }

    void MxObject::Destroy(MxObject::Handle object)
    {
        if(object.IsValid()) Factory<MxObject>::Destroy(object);
    }

    void MxObject::Destroy(MxObject& object)
    {
        MX_ASSERT(object.handle != InvalidHandle);
        Factory<MxObject>::GetPool().Deallocate(object.handle);
    }

    ComponentView<MxObject> MxObject::GetObjects()
    {
        return ComponentView<MxObject>{ Factory<MxObject>::GetPool() };
    }

    MxObject::Handle MxObject::GetByName(const MxString& name)
    {
        auto& factory = Factory<MxObject>::GetPool();
        for (auto& resource : factory)
        {
            if (resource.value.Name == name)
                return MxObject::Handle{ resource.uuid, factory.IndexOf(resource) };
        }
        return MxObject::Handle{ };
    }

    MxObject::Handle MxObject::GetHandle(const MxObject& object)
    {
        return MxObject::GetByHandle(object.GetNativeHandle());
    }

    MxObject::Handle MxObject::GetByHandle(EngineHandle handle)
    {
        MX_ASSERT(handle != InvalidHandle);
        auto& managedObject = Factory<MxObject>::GetPool()[handle];
        MX_ASSERT(managedObject.refCount > 0 && managedObject.uuid != UUIDGenerator::GetNull());
        return MxObject::Handle(managedObject.uuid, handle);
    }

    MxObject::EngineHandle MxObject::GetNativeHandle() const
    {
        return this->handle;
    }

    MxObject::~MxObject()
    {
        this->components.RemoveAllComponents();
    }
}