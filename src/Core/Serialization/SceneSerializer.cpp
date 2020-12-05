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
#include "Core/Components/Instancing/Instance.h"
#include "Core/Serialization/DeserializerMappings.h"

namespace MxEngine
{
    void SceneSerializer::SerializeGlobals(JsonFile& json)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::SerializeGlobals()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::SerializeGlovals()");

        auto viewport = Rendering::GetViewport();

        json["globals"]["viewport-id"    ] = viewport.IsValid() ? viewport.GetHandle() : size_t(-1);
        json["globals"]["light-samples"  ] = Rendering::GetLightSamples();
        json["globals"]["blur-iterations"] = Rendering::GetShadowBlurIterations();
        json["globals"]["overlay-debug"  ] = Rendering::IsDebugOverlayed();
        json["globals"]["paused"         ] = Runtime::IsApplicationPaused();
        json["globals"]["time-scale"     ] = Runtime::GetApplicationTimeScale();
        json["globals"]["gravity"        ] = Physics::GetGravity();
        json["globals"]["physics-step"   ] = Physics::GetSimulationStep();
        json["globals"]["total-time"     ] = Time::Current();
    }

    void SceneSerializer::SerializeObjects(JsonFile& json)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::SerializeObjects()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::SerializeObjects()");

        auto& objects = json["mxobjects"];
        auto view = MxObject::GetObjects();
        for (auto& object : view)
        {
            if (object.IsSerialized && !object.HasComponent<Instance>())
            {
                auto& j = objects.emplace_back();
                MxEngine::Serialize(j, object);
            }
        }
    }

    void SceneSerializer::SerializeResources(JsonFile& json)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::SerializeResources()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::SerializeResources()");

        SerializeAudios   (json["audios"   ]);
        SerializeCubeMaps (json["cubemaps" ]);
        SerializeMaterials(json["materials"]);
        SerializeMeshes   (json["meshes"   ]);
        SerializeTextures (json["textures" ]);
        SerializeScripts  (json["scripts"  ]);
    }

    void SceneSerializer::DeserializeGlobals(const JsonFile& json, DeserializerMappings& mappings)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::DeserializeGlobals()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::DeserializeGlobals()");

        Rendering::SetViewport(mappings.CameraControllers[json["globals"]["viewport-id"]]);
        Rendering::SetLightSamples(        json["globals"]["light-samples"  ]);
        Rendering::SetShadowBlurIterations(json["globals"]["blur-iterations"]);
        Rendering::SetDebugOverlay(        json["globals"]["overlay-debug"  ]);
        Runtime::SetApplicationPaused(     json["globals"]["paused"         ]);
        Runtime::SetApplicationTimeScale(  json["globals"]["time-scale"     ]);
        Physics::SetGravity(               json["globals"]["gravity"        ]);
        Physics::SetSimulationStep(        json["globals"]["physics-step"   ]);
        Runtime::SetApplicationTotalTime(  json["globals"]["total-time"     ]);
    }

    void SceneSerializer::DeserializeObjects(const JsonFile& json, DeserializerMappings& mappings)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::DeserializeObjects()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::DeserializeObjects()");

        auto objects = MxObject::GetObjects();
        for (auto& object : objects)
            MxObject::Destroy(object);

        // TODO: create new MxObjects
    }

    void SceneSerializer::DeserializeResources(const JsonFile& json, DeserializerMappings& mappings)
    {
        MAKE_SCOPE_PROFILER("SceneSerializer::DeserializeResources()");
        MAKE_SCOPE_TIMER("MxEngine::SceneSerializer", "SceneSerializer::DeserializeResources()");

        ClearExistingResources();

        DeserializeTextures( json["textures" ], mappings);
        DeserializeMaterials(json["materials"], mappings);
        DeserializeCubeMaps( json["cubemaps" ], mappings);
        DeserializeMeshes(   json["meshes"   ], mappings);
        DeserializeScripts(  json["scripts"  ], mappings);
        DeserializeAudios(   json["audios"   ], mappings);
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

        DeserializerMappings mappings;

        SceneSerializer::DeserializeResources(json, mappings);
        SceneSerializer::DeserializeObjects(json, mappings);
        SceneSerializer::DeserializeGlobals(json, mappings);
    }
}