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

#include "Core/Serialization/SceneSerializer.h"
#include "Core/Application/Runtime.h"
#include "Core/Resources/AssetManager.h"

namespace MxEngine
{
    void SerializeScripts(JsonFile& json)
    {
        auto& scripts = Runtime::GetRegisteredScripts();
        for (auto it = scripts.begin(); it != scripts.end(); it++)
        {
            auto& script = it->second;
            auto& j = json.emplace_back();
            j["name"] = script.Name;
            j["filepath"] = script.FilePath;
        }
    }

    void SerializeMaterials(JsonFile& json)
    {
        auto& materials = ResourceFactory::Get<Material>();
        for (const auto& material : materials)
        {
            auto& j = json.emplace_back();
            j["id"] = materials.IndexOf(material);

            j["albedo-map-id"]    = material.value.AlbedoMap.IsValid()           ? material.value.AlbedoMap.GetHandle()           : size_t(-1);
            j["emmisive-map-id"]  = material.value.EmmisiveMap.IsValid()         ? material.value.EmmisiveMap.GetHandle()         : size_t(-1);
            j["height-map-id"]    = material.value.HeightMap.IsValid()           ? material.value.HeightMap.GetHandle()           : size_t(-1);
            j["normal-map-id"]    = material.value.NormalMap.IsValid()           ? material.value.NormalMap.GetHandle()           : size_t(-1);
            j["ao-map-id"]        = material.value.AmbientOcclusionMap.IsValid() ? material.value.AmbientOcclusionMap.GetHandle() : size_t(-1);
            j["metallic-map-id"]  = material.value.MetallicMap.IsValid()         ? material.value.MetallicMap.GetHandle()         : size_t(-1);
            j["roughness-map-id"] = material.value.RoughnessMap.IsValid()        ? material.value.RoughnessMap.GetHandle()        : size_t(-1);

            j["transparency"]  = material.value.Transparency;
            j["emmision"]      = material.value.Emmision;
            j["displacement"]  = material.value.Displacement;
            j["roughness"]     = material.value.RoughnessFactor;
            j["metallic"]      = material.value.MetallicFactor;

            j["base-color"]    = material.value.BaseColor;
            j["uv-mult"]       = material.value.UVMultipliers;
            j["name"]          = material.value.Name;
        }
    }

    template<typename T>
    bool ShouldBeSerialized(const T& resource)
    {
        const MxString& filepath = resource.GetFilePath();
        if (filepath.empty()) return false;

        // internal engine resource
        if (filepath.find("[[") != filepath.npos && filepath.find("]]") != filepath.npos)
            return false;

        return true;
    }

    void SerializeMeshes(JsonFile& json)
    {
        auto& meshes = ResourceFactory::Get<Mesh>();
        for (const auto& mesh : meshes)
        {
            auto& j = json.emplace_back();
            j["id"] = meshes.IndexOf(mesh);

            j["filepath"] = ShouldBeSerialized(mesh.value) ? mesh.value.GetFilePath() : "";
        }
    }

    void SerializeAudios(JsonFile& json)
    {
        auto& audios = AudioFactory::Get<AudioBuffer>();
        for (const auto& audio : audios)
        {
            auto& j = json.emplace_back();
            j["id"] = audios.IndexOf(audio);

            j["filepath"] = ShouldBeSerialized(audio.value) ? audio.value.GetFilePath() : "";
        }
    }

    void SerializeCubeMaps(JsonFile& json)
    {
        auto& cubemaps = GraphicFactory::Get<CubeMap>();
        for (const auto& cubemap : cubemaps)
        {
            auto& j = json.emplace_back();
            j["id"] = cubemaps.IndexOf(cubemap);

            j["filepath"] = ShouldBeSerialized(cubemap.value) ? cubemap.value.GetFilePath() : "";
        }
    }

    void SerializeTextures(JsonFile& json)
    {
        auto& textures = GraphicFactory::Get<Texture>();
        for (const auto& texture : textures)
        {
            auto& j = json.emplace_back();
            j["id"] = textures.IndexOf(texture);

            j["filepath"] = ShouldBeSerialized(texture.value) ? texture.value.GetFilePath() : "";
            j["wrap"] = texture.value.GetWrapType();
            j["format"] = texture.value.GetFormat();
        }
    }
}