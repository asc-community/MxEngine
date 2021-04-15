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

#include "Serialization.h"
#include "Core/Components/Scripting/Script.h"
#include "Core/Components/Camera/CameraController.h"
#include "Core/Resources/AssetManager.h"
#include "Core/Runtime/RuntimeCompiler.h"
#include "Core/Runtime/HandleMappings.h"

namespace MxEngine
{
    template<typename Handle>
    void SerializeResourceImpl(JsonFile& json)
    {
        using Type = typename Handle::Type;
        using Factory = typename Handle::Factory;

        auto type = rttr::type::get<Type>();
        const char* name = type.get_name().cbegin();
        auto& jlist = json[name];
        auto& pool = Factory::GetPool();

        for (const auto& resource : pool)
        {
            auto& object = resource.value;
            if (!object.IsInternalEngineResource())
            {
                auto& j = jlist.emplace_back();
                j["id"] = pool.IndexOf(resource);
                Serialize(j, object);
            }
        }
    }

    template<typename Handle>
    void DeserializeResourceImpl(const JsonFile& json, HandleMappings& mappings)
    {
        using Type = typename Handle::Type;
        using Factory = typename Handle::Factory;

        auto handleType = rttr::type::get<Handle>();
        auto objectType = rttr::type::get<Type>();
        const char* name = objectType.get_name().cbegin();
        auto& jlist = json[name];

        mappings.TypeToProjection.insert(handleType);

        for (const auto& jresource : jlist)
        {
            auto resource = Factory::Create();
            Deserialize(jresource, *resource, mappings);

            size_t jsonHandleId = jresource["id"];
            mappings.TypeToProjection.at(handleType)[jsonHandleId] = rttr::variant{ resource };
        }
    }

    template<>
    void SerializeResourceImpl<Script::Handle>(JsonFile& json)
    {
        auto& pool = RuntimeCompiler::GetRegisteredScripts();
        auto& jlist = json["Script"];
        for (const auto& info : pool)
        {
            auto& j = jlist.emplace_back();
            j["name"] = info.second.Name;
            j["filepath"] = info.second.FilePath;
        }
    }

    template<>
    void DeserializeResourceImpl<Script::Handle>(const JsonFile& json, HandleMappings& mappings)
    {
        auto& jlist = json["Script"];

        bool needRecompile = false;
        for (const auto& jscript : jlist)
        {
            auto name = jscript["name"].get<MxString>();
            auto filepath = jscript["filepath"].get<MxString>();

            if (!RuntimeCompiler::HasScript(name) || RuntimeCompiler::GetScriptInfo(name).FilePath != filepath)
            {
                RuntimeCompiler::AddScriptFile(name, ToFilePath(filepath));
                needRecompile = true;
            }
        }
        if(needRecompile) RuntimeCompiler::StartCompilationTask();
    }

    template<> void SerializeResource<Script>     (JsonFile& json) { SerializeResourceImpl<Script::Handle>   (json); }
    template<> void SerializeResource<Mesh>       (JsonFile& json) { SerializeResourceImpl<MeshHandle>       (json); }
    template<> void SerializeResource<Texture>    (JsonFile& json) { SerializeResourceImpl<TextureHandle>    (json); }
    template<> void SerializeResource<CubeMap>    (JsonFile& json) { SerializeResourceImpl<CubeMapHandle>    (json); }
    template<> void SerializeResource<AudioBuffer>(JsonFile& json) { SerializeResourceImpl<AudioBufferHandle>(json); }
    template<> void SerializeResource<Material>   (JsonFile& json) { SerializeResourceImpl<MaterialHandle>   (json); }

    template<> void DeserializeResource<Script>     (const JsonFile& json, HandleMappings& mappings) { DeserializeResourceImpl<Script::Handle>   (json, mappings); }
    template<> void DeserializeResource<Mesh>       (const JsonFile& json, HandleMappings& mappings) { DeserializeResourceImpl<MeshHandle>       (json, mappings); }
    template<> void DeserializeResource<Texture>    (const JsonFile& json, HandleMappings& mappings) { DeserializeResourceImpl<TextureHandle>    (json, mappings); }
    template<> void DeserializeResource<CubeMap>    (const JsonFile& json, HandleMappings& mappings) { DeserializeResourceImpl<CubeMapHandle>    (json, mappings); }
    template<> void DeserializeResource<AudioBuffer>(const JsonFile& json, HandleMappings& mappings) { DeserializeResourceImpl<AudioBufferHandle>(json, mappings); }
    template<> void DeserializeResource<Material>   (const JsonFile& json, HandleMappings& mappings) { DeserializeResourceImpl<MaterialHandle>   (json, mappings); }
}