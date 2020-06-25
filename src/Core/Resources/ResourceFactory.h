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

namespace MxEngine
{
    using ResourceFactory = AbstractFactoryImpl<Material, Mesh>;

    using MaterialHandle = Resource<Material, ResourceFactory>;
    using MeshHandle = Resource<Mesh, ResourceFactory>;

    class AssetManager
    {
    public:
        static GResource<CubeMap> LoadCubeMap(StringId hash);
        static GResource<CubeMap> LoadCubeMap(const FilePath& path);
        static GResource<CubeMap> LoadCubeMap(const MxString& path);
        static GResource<CubeMap> LoadCubeMap(const char* path);

        static GResource<Texture> LoadTexture(StringId hash);
        static GResource<Texture> LoadTexture(const FilePath& path);
        static GResource<Texture> LoadTexture(const MxString& path);
        static GResource<Texture> LoadTexture(const char* path);

        static GResource<Shader> LoadShader(StringId vertex, StringId fragment);
        static GResource<Shader> LoadShader(const FilePath& vertex, const FilePath& fragment);
        static GResource<Shader> LoadShader(const MxString& vertex, const MxString& fragment);
        static GResource<Shader> LoadShader(const char* vertex, const char* fragment);
        static GResource<Shader> LoadShader(StringId vertex, StringId geometry, StringId fragment);
        static GResource<Shader> LoadShader(const FilePath& vertex, const FilePath& geometry, const FilePath& fragment);
        static GResource<Shader> LoadShader(const MxString& vertex, const MxString& geometry, const MxString& fragment);
        static GResource<Shader> LoadShader(const char* vertex, const char* geometry, const char* fragment);

        static Resource<Mesh, ResourceFactory> LoadMesh(StringId hash);
        static Resource<Mesh, ResourceFactory> LoadMesh(const FilePath& path);
        static Resource<Mesh, ResourceFactory> LoadMesh(const MxString& path);
        static Resource<Mesh, ResourceFactory> LoadMesh(const char* path);

        static MxVector<Resource<Material, ResourceFactory>> LoadMaterials(StringId hash);
        static MxVector<Resource<Material, ResourceFactory>> LoadMaterials(const FilePath& path);
        static MxVector<Resource<Material, ResourceFactory>> LoadMaterials(const MxString& path);
        static MxVector<Resource<Material, ResourceFactory>> LoadMaterials(const char* path);
    };
}