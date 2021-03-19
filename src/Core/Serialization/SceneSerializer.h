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

#include "Serialization.h"
#include "Cloning.h"
#include "Utilities/Logging/Logger.h"
#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
    struct SceneSerializerImpl
    {
        struct CloneCallback
        {
            rttr::type ComponentType;
            void(*Function)(const MxObject::Handle&, MxObject::Handle&);
        };

        MxVector<void(*)(JsonFile&, MxObject&)> serializeCallbacks;
        MxVector<void(*)(const JsonFile&, MxObject&, HandleMappings&)> deserializeCallbacks;
        MxVector<CloneCallback> cloneCallbacks;
    };

    class SceneSerializer
    {
        inline static SceneSerializerImpl* impl = nullptr;

        static void SerializeGlobals(JsonFile& json);
        static void SerializeObjects(JsonFile& json);
        static void SerializeResources(JsonFile& json);

        static void ClearResources();
        static HandleMappings DeserializeResources(const JsonFile& json);
        static void DeserializeGlobals(const JsonFile& json, const HandleMappings& mappings);
        static void DeserializeObjects(const JsonFile& json, HandleMappings& mappings);
    public:
        static void Init();
        static void Destroy();
        static void Clone(SceneSerializerImpl* other);
        static SceneSerializerImpl* GetImpl();

        static JsonFile Serialize();
        static void Deserialize(const JsonFile& scene);

        static JsonFile SerializeMxObject(MxObject& object);
        static void DeserializeMxObject(const JsonFile& json, MxObject::Handle object, HandleMappings& mappings);

        static void CloneMxObjectAsCopy(const MxObject::Handle& origin, MxObject::Handle& target);
        static void CloneMxObjectAsInstance(const MxObject::Handle& origin, MxObject::Handle& target);

        template<typename T>
        static void RegisterComponentAsCloneable()
        {
            if constexpr (std::is_default_constructible<T>::value)
            {
                impl->cloneCallbacks.push_back({ rttr::type::get<T>(),
                    [](const MxObject::Handle& origin, MxObject::Handle& target)
                    {
                        auto componentOrigin = origin->GetComponent<T>();
                        if (componentOrigin.IsValid())
                        {
                            auto componentTarget = target->GetOrAddComponent<T>();
                            CloneComponent(*componentOrigin, *componentTarget);
                        }
                    }
                });
            }
            else
            {
                impl->cloneCallbacks.push_back({ rttr::type::get<T>(),
                    [](const MxObject::Handle& origin, MxObject::Handle& target)
                    {
                        auto componentOrigin = origin->GetComponent<T>();
                        if (componentOrigin.IsValid())
                        {
                            auto componentTarget = target->GetComponent<T>();
                            if(componentTarget.IsValid()) CloneComponent(*componentOrigin, *componentTarget);
                        }
                    }
                });
            }
        }

        template<typename T>
        static void RegisterComponent()
        {
            MXLOG_DEBUG("MxEngine::SceneSerializer", "registered component " + MxString(rttr::type::get<T>().get_name().cbegin()));

            impl->serializeCallbacks.push_back(
                [](JsonFile& json, MxObject& object)
                {
                    auto component = object.template GetComponent<T>();
                    if (component.IsValid())
                    {
                        rttr::type t = rttr::type::get<T>();
                        const char* name = t.get_name().cbegin();
                        SerializeComponent(json[name], component);
                    }
                }
            );

            if constexpr (std::is_default_constructible_v<T>)
            {
                impl->deserializeCallbacks.push_back(
                    [](const JsonFile& json, MxObject& object, HandleMappings& mappings)
                    {
                        rttr::type t = rttr::type::get<T>();
                        const char* name = t.get_name().cbegin();
                        if (json.contains(name))
                        {
                            auto component = object.template AddComponent<T>();
                            DeserializeComponent(json[name], component, mappings);
                        }
                    }
                );
            }
        }
    };

    MxObject::Handle Clone(MxObject::Handle object);
    void CloneCopyInternal(const MxObject::Handle& origin, MxObject::Handle& target);
    void CloneInstanceInternal(const MxObject::Handle& origin, MxObject::Handle& target);
}