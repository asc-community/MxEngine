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

#include "SceneSerializer.h"
#include "Core/Application/Rendering.h"
#include "Core/Application/Runtime.h"
#include "Core/Application/Physics.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/HandleMappings.h"

namespace MxEngine
{
    class Mesh;
    class Script;
    class Material;
    class Texture;
    class CubeMap;
    class AudioBuffer;

    bool IsInstance(const MxObject&);

    void SceneSerializer::SerializeGlobals(JsonFile& json)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::SerializeGlobals()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::SerializeGlobals()");

        auto viewport = Rendering::GetViewport();
        if (viewport.IsValid()) json["globals"]["viewport"] = viewport.GetHandle();

        json["globals"]["overlay-debug"] = Rendering::IsDebugOverlayed();
        json["globals"]["paused"       ] = Runtime::IsApplicationPaused();
        json["globals"]["time-scale"   ] = Runtime::GetApplicationTimeScale();
        json["globals"]["gravity"      ] = Physics::GetGravity();
        json["globals"]["physics-step" ] = Physics::GetSimulationStep();
        json["globals"]["total-time"   ] = Time::Current();
    }

    void SceneSerializer::SerializeObjects(JsonFile& json)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::SerializeObjects()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::SerializeObjects()");

        auto& objects = json["mxobjects"];
        auto view = MxObject::GetObjects();
        for (auto& object : view)
        {
            if (object.IsSerialized && !IsInstance(object))
            {
                objects.push_back(SceneSerializer::SerializeMxObject(object));
            }
        }
    }

    void SceneSerializer::SerializeResources(JsonFile& json)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::SerializeResources()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::SerializeResources()");

        SerializeResource<Script     >(json);
        SerializeResource<Mesh       >(json);
        SerializeResource<Material   >(json);
        SerializeResource<Texture    >(json);
        SerializeResource<CubeMap    >(json);
        SerializeResource<AudioBuffer>(json);
    }

    void SceneSerializer::DeserializeGlobals(const JsonFile& json, const HandleMappings& mappings)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::DeserializeGlobals()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::DeserializeGlobals()");

        if (json["globals"].contains("viewport"))
        {
            auto viewport = GetHandleById<CameraControllerHandle>(json["globals"]["viewport"], mappings);
            Rendering::SetViewport(viewport);
        }

        Rendering::SetDebugOverlay(      json["globals"]["overlay-debug"  ]);
        Runtime::SetApplicationPaused(   json["globals"]["paused"         ]);
        Runtime::SetApplicationTimeScale(json["globals"]["time-scale"     ]);
        Physics::SetGravity(             json["globals"]["gravity"        ]);
        Physics::SetSimulationStep(      json["globals"]["physics-step"   ]);
        Runtime::SetApplicationTotalTime(json["globals"]["total-time"     ]);
    }

    void SceneSerializer::DeserializeObjects(const JsonFile& json, HandleMappings& mappings)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::DeserializeObjects()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::DeserializeObjects()");

        const auto& jsonObjects = json["mxobjects"];
        for (const auto& entry : jsonObjects)
        {
            (void)SceneSerializer::DeserializeMxObject(entry, MxObject::Create(), mappings);
        }
    }

    void SceneSerializer::ClearResources()
    {
        // destroy all existing MxObjects
        auto objects = MxObject::GetObjects();
        for (auto& object : objects)
            MxObject::Destroy(object);

        // remove attached viewport
        Rendering::SetViewport(CameraControllerHandle{ });
    }

    HandleMappings SceneSerializer::DeserializeResources(const JsonFile& json)
    {
        HandleMappings mappings;

        MAKE_SCOPE_PROFILER("SceneSerializer::DeserializeResources()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::DeserializeResources()");

        DeserializeResource<Script     >(json, mappings);
        DeserializeResource<Mesh       >(json, mappings);
        DeserializeResource<Texture    >(json, mappings);
        DeserializeResource<CubeMap    >(json, mappings);
        DeserializeResource<AudioBuffer>(json, mappings);
        DeserializeResource<Material   >(json, mappings);

        // wait until all scripts are loaded and update script database
        while (RuntimeCompiler::HasCompilationTaskInProcess())
        {
            Time::Sleep(1.0f);
            RuntimeCompiler::OnUpdate(1.0f);
        }

        return mappings;
    }

    JsonFile SceneSerializer::Serialize()
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::Serialize()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::Serialize()");

        JsonFile json;

        SceneSerializer::SerializeGlobals(json);
        SceneSerializer::SerializeObjects(json);
        SceneSerializer::SerializeResources(json);

        return json;
    }

    void SceneSerializer::Deserialize(const JsonFile& json)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::Deserialize()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::Deserialize()");

        SceneSerializer::ClearResources();
        auto mappings = SceneSerializer::DeserializeResources(json);
        SceneSerializer::DeserializeObjects(json, mappings);
        SceneSerializer::DeserializeGlobals(json, mappings);
    }

    JsonFile SceneSerializer::SerializeMxObject(MxObject& object)
    {
        JsonFile json;
        json["name"] = object.Name;
        json["displayed"] = object.IsDisplayedInEditor;

        MxEngine::Serialize(json["transform"], object.LocalTransform);

        for (const auto& callback : impl->serializeCallbacks)
        {
            callback(json, object);
        }

        return json;
    }

    void SceneSerializer::DeserializeMxObject(const JsonFile& json, MxObject::Handle object, HandleMappings& mappings)
    {
        object->Name = json["name"].get<MxString>();
        object->IsDisplayedInEditor = json["displayed"];

        MxEngine::Deserialize(json["transform"], object->LocalTransform, mappings);

        for (const auto& callback : impl->deserializeCallbacks)
        {
            callback(json, *object, mappings);
        }
    }

    void SceneSerializer::CloneMxObjectAsCopy(const MxObject::Handle& origin, MxObject::Handle& target)
    {
        target->LocalTransform = origin->LocalTransform;
        target->Name = origin->Name;
        target->IsDisplayedInEditor = origin->IsDisplayedInEditor;
        target->IsSerialized = origin->IsSerialized;

        for (const auto& [type, callback] : impl->cloneCallbacks)
        {
            ReflectionMeta meta(type);
            if (meta.Flags & MetaInfo::CLONE_COPY)
                callback(origin, target);
        }
    }

    void SceneSerializer::CloneMxObjectAsInstance(const MxObject::Handle& origin, MxObject::Handle& target)
    {
        target->Name = origin->Name + "_instance";
        target->IsDisplayedInEditor = origin->IsDisplayedInEditor;
        target->IsSerialized = origin->IsSerialized;

        for (const auto& [type, callback] : impl->cloneCallbacks)
        {
            ReflectionMeta meta(type);
            if (meta.Flags & MetaInfo::CLONE_INSTANCE)
                callback(origin, target);
        }
    }

    void SceneSerializer::Init()
    {
        impl = Alloc<SceneSerializerImpl>();
    }

    void SceneSerializer::Destroy()
    {
        delete impl;
        impl = nullptr;
    }

    void SceneSerializer::Clone(SceneSerializerImpl* other)
    {
        impl = other;
    }

    SceneSerializerImpl* SceneSerializer::GetImpl()
    {
        return impl;
    }

    MxObject::Handle Clone(MxObject::Handle object)
    {
        auto clone = MxObject::Create();
        clone->Name = object->Name;
        SceneSerializer::CloneMxObjectAsCopy(object, clone);
        return clone;
    }

    void CloneCopyInternal(const MxObject::Handle& origin, MxObject::Handle& target)
    {
        SceneSerializer::CloneMxObjectAsCopy(origin, target);
    }

    void CloneInstanceInternal(const MxObject::Handle& origin, MxObject::Handle& target)
    {
        SceneSerializer::CloneMxObjectAsInstance(origin, target);
    }
}