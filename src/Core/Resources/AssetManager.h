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

#include "Utilities/AbstractFactory/AbstractFactory.h"
#include "Core/Resources/Mesh.h"
#include "Core/Resources/Material.h"
#include "Platform/GraphicAPI.h"
#include "Platform/AudioAPI.h"

namespace MxEngine
{
    using ResourceFactory = AbstractFactoryImpl<Material, Mesh>;

    using MaterialHandle = Resource<Material, ResourceFactory>;
    using MeshHandle = Resource<Mesh, ResourceFactory>;

    class AssetManager
    {
    public:
        static CubeMapHandle LoadCubeMap(StringId hash);
        static CubeMapHandle LoadCubeMap(const FilePath& path);
        static CubeMapHandle LoadCubeMap(const MxString& path);
        static CubeMapHandle LoadCubeMap(const char* path);

        static TextureHandle LoadTexture(StringId hash);
        static TextureHandle LoadTexture(const FilePath& path);
        static TextureHandle LoadTexture(const MxString& path);
        static TextureHandle LoadTexture(const char* path);

        static ShaderHandle LoadShader(StringId vertex, StringId fragment);
        static ShaderHandle LoadShader(const FilePath& vertex, const FilePath& fragment);
        static ShaderHandle LoadShader(const MxString& vertex, const MxString& fragment);
        static ShaderHandle LoadShader(const char* vertex, const char* fragment);
        static ShaderHandle LoadShader(StringId vertex, StringId geometry, StringId fragment);
        static ShaderHandle LoadShader(const FilePath& vertex, const FilePath& geometry, const FilePath& fragment);
        static ShaderHandle LoadShader(const MxString& vertex, const MxString& geometry, const MxString& fragment);
        static ShaderHandle LoadShader(const char* vertex, const char* geometry, const char* fragment);

        static MeshHandle LoadMesh(StringId hash);
        static MeshHandle LoadMesh(const FilePath& path);
        static MeshHandle LoadMesh(const MxString& path);
        static MeshHandle LoadMesh(const char* path);

        static MxVector<MaterialHandle> LoadMaterials(StringId hash);
        static MxVector<MaterialHandle> LoadMaterials(const FilePath& path);
        static MxVector<MaterialHandle> LoadMaterials(const MxString& path);
        static MxVector<MaterialHandle> LoadMaterials(const char* path);

        static AudioBufferHandle LoadAudio(StringId hash);
        static AudioBufferHandle LoadAudio(const FilePath& path);
        static AudioBufferHandle LoadAudio(const MxString& path);
        static AudioBufferHandle LoadAudio(const char* path);
    };
}