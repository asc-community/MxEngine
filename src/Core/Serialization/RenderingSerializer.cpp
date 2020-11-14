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

#include "Core/Components/Rendering/DebugDraw.h"
#include "Core/Components/Rendering/MeshLOD.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Serialization/SceneSerializer.h"

namespace MxEngine
{
    void Serialize(JsonFile& json, const DebugDraw& debug)
    {
        json["box-color"] = debug.BoundingBoxColor;
        json["sphere-color"] = debug.BoundingSphereColor;
        json["frustrum-color"] = debug.FrustrumColor;
        json["light-color"] = debug.LightSourceColor;
        json["sound-color"] = debug.SoundSourceColor;
        json["render-box"] = debug.RenderBoundingBox;
        json["render-sphere"] = debug.RenderBoundingSphere;
        json["render-frustrum"] = debug.RenderFrustrumBounds;
        json["render-light"] = debug.RenderLightingBounds;
        json["render-physics"] = debug.RenderPhysicsCollider;
        json["render-sounds"] = debug.RenderSoundBounds;
    }

    void Serialize(JsonFile& json, const MeshLOD& lod)
    {
        json["auto-select"] = lod.AutoLODSelection;
        json["current-lod"] = lod.CurrentLOD;
        // TODO: should LODs meshes be also serialized?
    }

    void Serialize(JsonFile& json, const Skybox& skybox)
    {
        json["intensity"] = skybox.GetIntensity();
        json["rotation"] = skybox.GetRotation();
        json["irradiance-id"] = skybox.Irradiance.GetHandle();
        json["cubemap-id"] = skybox.CubeMap.GetHandle();
    }

    void Serialize(JsonFile& json, const MeshSource& source)
    {
        json["is-drawn"] = source.IsDrawn;
        json["mesh-id"] = source.Mesh.GetHandle();
    }

    void Serialize(JsonFile& json, const MeshRenderer& renderer)
    {
        auto& jMaterials = json["material-ids"];
        for (auto& material : renderer.Materials)
        {
            jMaterials.emplace_back(material.GetHandle());
        }
    }
}