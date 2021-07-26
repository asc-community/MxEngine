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

#include "Utilities/Json/Json.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    struct HandleMappings;

    void Serialize(JsonFile& json, rttr::instance object);
    void Deserialize(const JsonFile& json, rttr::instance object, HandleMappings& mappings);
    void DeserializeComponentImpl(const JsonFile& json, rttr::instance object, rttr::variant handle, HandleMappings& mappings);
    rttr::variant VisitDeserialize(const JsonFile& json, const rttr::variant& property, HandleMappings& mappings);
    void VisitSerialize(JsonFile& json, const rttr::variant& v, const ReflectionMeta& meta);

    template<typename Handle>
    void SerializeComponent(JsonFile& json, Handle component)
    {
        json["id"] = component.GetHandle();
        Serialize(json, rttr::instance{ *component });
    }

    template<typename Handle>
    void DeserializeComponent(const JsonFile& json, Handle component, HandleMappings& mappings)
    {
        DeserializeComponentImpl(json, rttr::instance{ *component }, rttr::variant{ component }, mappings);
    }

    template<typename T>
    void SerializeResource(JsonFile& json);

    template<typename T>
    void DeserializeResource(const JsonFile& json, HandleMappings& mappings);
}