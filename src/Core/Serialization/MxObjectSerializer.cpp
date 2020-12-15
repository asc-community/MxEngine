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

#include "Core/MxObject/MxObject.h"
#include "Core/Serialization/SceneSerializer.h"
#include "Core/Serialization/DeserializerMappings.h"
#include "Core/Components/Components.h"

namespace MxEngine
{
    template<typename T>
    void SerializeComponent(JsonFile& json, const MxObject& object)
    {
        CResource<T> c = object.GetComponent<T>();
        if (c.IsValid()) 
        {
            json["id"] = c.GetHandle();
            Serialize(json, *c);
        }
    }

    template<typename T>
    void DeserializeComponent(const JsonFile& json, DeserializerMappings& mappings, MxObject& object)
    {
        if (json.empty()) return;
        CResource<T> c = object.AddComponent<T>();
        Deserialize(json, mappings, *c);
    }

    //template<>
    //void DeserializeComponent<Instance>(const JsonFile& json, DeserializerMappings& mappings, MxObject& object)
    //{
    //    if (json.empty()) return;
    //    CResource<Instance> c = object.GetComponent<Instance>();
    //    Deserialize(json, mappings, *c);
    //}

    using SerializeCallback = void(*)(JsonFile&, const MxObject&);
    using DeserializeCallback = void(*)(const JsonFile&, DeserializerMappings&, MxObject&);
    using SerializerPair = std::pair<SerializeCallback, DeserializeCallback>;

    #define REGISTER(component) std::make_pair<SerializeCallback, DeserializeCallback>(\
         [](JsonFile& json, const MxObject& object) { SerializeComponent<component>(json[#component], object); },\
         [](const JsonFile& json, DeserializerMappings& mappings, MxObject& object) { DeserializeComponent<component>(json[#component], mappings, object); })

    std::array callbacks =
    {
        //REGISTER(Behaviour),
        //REGISTER(DirectionalLight),
        //REGISTER(SpotLight),
        //REGISTER(PointLight),
        //REGISTER(Instance),
        //REGISTER(InstanceFactory),
        //REGISTER(CameraController),
        //REGISTER(CameraEffects),
        //REGISTER(CameraToneMapping),
        //REGISTER(VRCameraController),
        //REGISTER(InputController),
        //REGISTER(CameraSSR),
        //REGISTER(MeshRenderer),
        //REGISTER(MeshSource),
        //REGISTER(DebugDraw),
        //REGISTER(Skybox),
        //REGISTER(MeshLOD),
        REGISTER(AudioListener),
        //REGISTER(AudioSource),
        //REGISTER(RigidBody),
        //REGISTER(CharacterController),
        //REGISTER(BoxCollider),
        //REGISTER(SphereCollider),
        //REGISTER(CapsuleCollider),
        //REGISTER(CylinderCollider),
        //REGISTER(CompoundCollider),
    };

    void Serialize(JsonFile& json, const MxObject& object)
    {
        json["id"] = object.GetNativeHandle();
        json["displayed"] = object.IsDisplayedInRuntimeEditor();
        json["name"] = object.Name;
        Serialize(json["transform"], object.Transform);
       
        auto& components = json["components"];
        for (const auto& [serialize, deserialize] : callbacks)
        {
            serialize(components, object);
        }
    }

    void Deserialize(const JsonFile& json, DeserializerMappings& mappings, MxObject& object)
    {
        object.SetDisplayInRuntimeEditor(json["displayed"]);
        object.Name = json["name"].get<MxString>();
        //Deserialize(json["transform"], mappings, object.Transform);

        auto& components = json["components"];
        for (const auto& [serialize, deserialize] : callbacks)
        {
            deserialize(components, mappings, object);
        }
    }
}