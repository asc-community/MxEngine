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

#include "Serializer.h"
#include "Utilities/Logging/Logger.h"
#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
    struct SceneSerializerImpl
    {
        MxVector<void(*)(JsonFile&, MxObject&)> serializeCallbacks;
    };

    class SceneSerializer
    {
        inline static SceneSerializerImpl* impl = nullptr;

        static void SerializeGlobals(JsonFile& json);
        static void SerializeObjects(JsonFile& json);
        static void SerializeResources(JsonFile& json);

        static void DeserializeGlobals(const JsonFile& json);
        static void DeserializeObjects(const JsonFile& json);
        static void DeserializeResources(const JsonFile& json);
    public:
        static void Init();
        static void Destroy();
        static void Clone(SceneSerializerImpl* other);
        static SceneSerializerImpl* GetImpl();

        static JsonFile Serialize();
        static void Deserialize(const JsonFile& scene);

        static JsonFile SerializeMxObject(MxObject& object);
        static MxObject& DeserializeMxObject(const JsonFile& json);

        template<typename T>
        static void RegisterComponent()
        {
            MXLOG_DEBUG("MxEngine::SceneSerializer", "registered component " + MxString(rttr::type::get<T>().get_name().cbegin()));

            impl->serializeCallbacks.push_back([](JsonFile& json, MxObject& object)
            {
                auto component = object.template GetComponent<T>();
                if (component.IsValid())
                {
                    rttr::type t = rttr::type::get<T>();
                    const char* name = t.get_name().cbegin();
                    MxEngine::Serialize(json[name], *component);
                }
            });
        }
    };
}