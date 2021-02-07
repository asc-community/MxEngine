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

#include "Serializer.h"
#include "Core/Components/Scripting/Script.h"
#include "Core/Resources/AssetManager.h"
#include "Core/Runtime/RuntimeCompiler.h"

namespace MxEngine
{
    template<typename T, typename Factory>
    void SerializeResourceImpl(JsonFile& json, Resource<T, Factory>)
    {
        const char* name = rttr::type::get<T>().get_name().cbegin();
        auto& jlist = json[name];
        auto& pool = Factory::template Get<T>();
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

    template<>
    void SerializeResourceImpl(JsonFile& json, Script::Handle)
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

    template<> void SerializeResource<Script>(JsonFile& json) { SerializeResourceImpl(json, Script::Handle{ }); }
    template<> void SerializeResource<Mesh>(JsonFile& json) { SerializeResourceImpl(json, MeshHandle{ }); }
    template<> void SerializeResource<Material>(JsonFile& json) { SerializeResourceImpl(json, MaterialHandle{ }); }
    template<> void SerializeResource<Texture>(JsonFile& json) { SerializeResourceImpl(json, TextureHandle{ }); }
    template<> void SerializeResource<CubeMap>(JsonFile& json) { SerializeResourceImpl(json, CubeMapHandle{ }); }
    template<> void SerializeResource<AudioBuffer>(JsonFile& json) { SerializeResourceImpl(json, AudioBufferHandle{ }); }
}