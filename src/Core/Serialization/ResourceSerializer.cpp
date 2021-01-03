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
#include "Core/Serialization/DeserializerMappings.h"
#include "Core/Application/Runtime.h"

namespace MxEngine
{
    bool IsInternalEngineResource(const MxString& filepath)
    {
        return filepath.size() < 2 || (filepath[0] == '[' && filepath[1] == '[');
    }

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

    void DeserializeScripts(const JsonFile& json, DeserializerMappings& mappings)
    {
        for (const auto& j : json)
        {
            FilePath path = j["filepath"];
            if (!path.empty())
            {
                RuntimeCompiler::AddScriptFile(j["name"], path);
            }
        }
    }

    void SerializeMaterials(JsonFile& json)
    {
        auto& materials = ResourceFactory::Get<Material>();
        for (const auto& material : materials)
        {
            auto& j = json.emplace_back();
            j["id"] = materials.IndexOf(material);

            j["albedo-map-id"   ] = material.value.AlbedoMap.IsValid()           ? material.value.AlbedoMap.GetHandle()           : size_t(-1);
            j["emissive-map-id" ] = material.value.EmissiveMap.IsValid()         ? material.value.EmissiveMap.GetHandle()         : size_t(-1);
            j["height-map-id"   ] = material.value.HeightMap.IsValid()           ? material.value.HeightMap.GetHandle()           : size_t(-1);
            j["normal-map-id"   ] = material.value.NormalMap.IsValid()           ? material.value.NormalMap.GetHandle()           : size_t(-1);
            j["ao-map-id"       ] = material.value.AmbientOcclusionMap.IsValid() ? material.value.AmbientOcclusionMap.GetHandle() : size_t(-1);
            j["metallic-map-id" ] = material.value.MetallicMap.IsValid()         ? material.value.MetallicMap.GetHandle()         : size_t(-1);
            j["roughness-map-id"] = material.value.RoughnessMap.IsValid()        ? material.value.RoughnessMap.GetHandle()        : size_t(-1);

            j["transparency"]  = material.value.Transparency;
            j["emission"]      = material.value.Emission;
            j["displacement"]  = material.value.Displacement;
            j["roughness"]     = material.value.RoughnessFactor;
            j["metallic"]      = material.value.MetallicFactor;

            j["base-color"]    = material.value.BaseColor;
            j["uv-mult"]       = material.value.UVMultipliers;
            j["name"]          = material.value.Name;
        }
    }

    void DeserializeMaterials(const JsonFile& json, DeserializerMappings& mappings)
    {
        for (const auto& j : json)
        {
            auto material = ResourceFactory::Create<Material>();

            material->AlbedoMap           = mappings.Textures[j["albedo-map-id"   ]];  
            material->EmissiveMap         = mappings.Textures[j["emissive-map-id" ]];
            material->HeightMap           = mappings.Textures[j["height-map-id"   ]];  
            material->NormalMap           = mappings.Textures[j["normal-map-id"   ]];  
            material->AmbientOcclusionMap = mappings.Textures[j["ao-map-id"       ]];      
            material->MetallicMap         = mappings.Textures[j["metallic-map-id" ]];
            material->RoughnessMap        = mappings.Textures[j["roughness-map-id"]];

            material->Transparency        = j["transparency"];
            material->Emission            = j["emission"];
            material->Displacement        = j["displacement"];
            material->RoughnessFactor     = j["roughness"];
            material->MetallicFactor      = j["metallic"];
                                          
            material->BaseColor           = j["base-color"];
            material->UVMultipliers       = j["uv-mult"];
            material->Name                = (MxString)j["name"];

            mappings.Materials[j["id"]] = material;
        }
    }

    void SerializeMeshes(JsonFile& json)
    {
        auto& meshes = ResourceFactory::Get<Mesh>();
        for (const auto& mesh : meshes)
        {
            auto& j = json.emplace_back();
            j["id"] = meshes.IndexOf(mesh);

            auto& path = mesh.value.GetFilePath();
            j["filepath"] = IsInternalEngineResource(path) ? "" : path;
        }
    }

    void DeserializeMeshes(const JsonFile& json, DeserializerMappings& mappings)
    {
        for (const auto& j : json)
        {
            FilePath path = j["filepath"];
            if (!path.empty())
            {
                auto mesh = ResourceFactory::Create<Mesh>();
                mesh->Load(path);
                mappings.Meshes[j["id"]] = mesh;
            }
        }
    }

    void SerializeAudios(JsonFile& json)
    {
        auto& audios = AudioFactory::Get<AudioBuffer>();
        for (const auto& audio : audios)
        {
            auto& j = json.emplace_back();
            j["id"] = audios.IndexOf(audio);

            auto& path = audio.value.GetFilePath();
            j["filepath"] = IsInternalEngineResource(path) ? "" : path;
        }
    }

    void DeserializeAudios(const JsonFile& json, DeserializerMappings& mappings)
    {
        for (const auto& j : json)
        {
            FilePath path = j["filepath"];
            if (!path.empty())
            {
                auto audio = AudioFactory::Create<AudioBuffer>();
                audio->Load(path);
                mappings.AudioBuffers[j["id"]] = audio;
            }
        }
    }

    void SerializeCubeMaps(JsonFile& json)
    {
        auto& cubemaps = GraphicFactory::Get<CubeMap>();
        for (const auto& cubemap : cubemaps)
        {
            auto& j = json.emplace_back();
            j["id"] = cubemaps.IndexOf(cubemap);

            auto& path = cubemap.value.GetFilePath();
            j["filepath"] = IsInternalEngineResource(path) ? "" : path;
        }
    }

    void DeserializeCubeMaps(const JsonFile& json, DeserializerMappings& mappings)
    {
        for (const auto& j : json)
        {
            FilePath path = j["filepath"];
            if (!path.empty())
            {
                auto cubemap = GraphicFactory::Create<CubeMap>();
                cubemap->Load(path);
                mappings.CubeMaps[j["id"]] = cubemap;
            }
        }
    }

    void SerializeTextures(JsonFile& json)
    {
        auto& textures = GraphicFactory::Get<Texture>();
        for (const auto& texture : textures)
        {
            auto& j = json.emplace_back();
            j["id"] = textures.IndexOf(texture);

            j["format"] = texture.value.GetFormat();
            j["wrap"] = texture.value.GetWrapType();
            
            auto& path = texture.value.GetFilePath();
            j["filepath"] = IsInternalEngineResource(path) ? "" : path;
        }
    }

    void DeserializeTextures(const JsonFile& json, DeserializerMappings& mappings)
    {
        for (const auto& j : json)
        {
            FilePath path = j["filepath"];
            if (!path.empty())
            {
                auto texture = GraphicFactory::Create<Texture>();
                texture->Load(path, j["format"], j["wrap"]);
                mappings.Textures[j["id"]] = texture;
            }
        }
    }

    void ClearExistingResources()
    {
        // just recreate compiler
        RuntimeCompiler::Destroy();
        RuntimeCompiler::Init();

        // all materials can be safely destroyed
        auto& materials = ResourceFactory::Get<Material>();
        for (auto& material : materials)
        {
            materials.Deallocate(materials.IndexOf(material));
        }

        auto& audios = AudioFactory::Get<AudioBuffer>();
        for (auto& audio : audios)
        {
            if (!IsInternalEngineResource(audio.value.GetFilePath()))
            {
                audios.Deallocate(audios.IndexOf(audio));
            }
        }

        auto& meshes = ResourceFactory::Get<Mesh>();
        for (auto& mesh : meshes)
        {
            if (!IsInternalEngineResource(mesh.value.GetFilePath()))
            {
                audios.Deallocate(meshes.IndexOf(mesh));
            }
        }

        auto& textures = GraphicFactory::Get<Texture>();
        for (auto& texture : textures)
        {
            if (!IsInternalEngineResource(texture.value.GetFilePath()))
            {
                textures.Deallocate(textures.IndexOf(texture));
            }
        }

        auto& cubemaps = GraphicFactory::Get<CubeMap>();
        for (auto& cubemap : cubemaps)
        {
            if (!IsInternalEngineResource(cubemap.value.GetFilePath()))
            {
                cubemaps.Deallocate(cubemaps.IndexOf(cubemap));
            }
        }
    }
}