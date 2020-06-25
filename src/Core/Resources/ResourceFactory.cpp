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

#include "ResourceFactory.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Core/Components/Rendering/MeshRenderer.h"

namespace MxEngine
{
    GResource<CubeMap> AssetManager::LoadCubeMap(StringId hash)
    {
        return AssetManager::LoadCubeMap(FileManager::GetFilePath(hash));
    }

    GResource<CubeMap> AssetManager::LoadCubeMap(const FilePath& path)
    {
        return AssetManager::LoadCubeMap(ToMxString(path));
    }

    GResource<CubeMap> AssetManager::LoadCubeMap(const MxString& path)
    {
        return GraphicFactory::Create<CubeMap>(path);
    }

    GResource<CubeMap> AssetManager::LoadCubeMap(const char* path)
    {
        return AssetManager::LoadCubeMap(MxString(path));
    }

    GResource<Texture> AssetManager::LoadTexture(StringId hash)
    {
        return AssetManager::LoadTexture(FileManager::GetFilePath(hash));
    }

    GResource<Texture> AssetManager::LoadTexture(const FilePath& path)
    {
        return AssetManager::LoadTexture(ToMxString(path));
    }

    GResource<Texture> AssetManager::LoadTexture(const MxString& path)
    {
        return GraphicFactory::Create<Texture>(path);
    }

    GResource<Texture> AssetManager::LoadTexture(const char* path)
    {
        return AssetManager::LoadTexture(MxString(path));
    }

    GResource<Shader> AssetManager::LoadShader(StringId vertex, StringId fragment)
    {
        return AssetManager::LoadShader(FileManager::GetFilePath(vertex), FileManager::GetFilePath(fragment));
    }

    GResource<Shader> AssetManager::LoadShader(const FilePath& vertex, const FilePath& fragment)
    {
        return AssetManager::LoadShader(ToMxString(vertex), ToMxString(fragment));
    }

    GResource<Shader> AssetManager::LoadShader(const MxString& vertex, const MxString& fragment)
    {
        return GraphicFactory::Create<Shader>(vertex, fragment);
    }

    GResource<Shader> AssetManager::LoadShader(const char* vertex, const char* fragment)
    {
        return AssetManager::LoadShader(MxString(vertex), MxString(fragment));
    }

    GResource<Shader> AssetManager::LoadShader(StringId vertex, StringId geometry, StringId fragment)
    {
        return AssetManager::LoadShader(FileManager::GetFilePath(vertex), FileManager::GetFilePath(geometry), FileManager::GetFilePath(fragment));
    }

    GResource<Shader> AssetManager::LoadShader(const FilePath& vertex, const FilePath& geometry, const FilePath& fragment)
    {
        return AssetManager::LoadShader(ToMxString(vertex), ToMxString(geometry), ToMxString(fragment));
    }

    GResource<Shader> AssetManager::LoadShader(const MxString& vertex, const MxString& geometry, const MxString& fragment)
    {
        return GraphicFactory::Create<Shader>(vertex, geometry, fragment);
    }

    GResource<Shader> AssetManager::LoadShader(const char* vertex, const char* geometry, const char* fragment)
    {
        return AssetManager::LoadShader(MxString(vertex), MxString(geometry), MxString(fragment));
    }

    Resource<Mesh, ResourceFactory> AssetManager::LoadMesh(StringId hash)
    {
        return AssetManager::LoadMesh(FileManager::GetFilePath(hash));
    }

    Resource<Mesh, ResourceFactory> AssetManager::LoadMesh(const FilePath& path)
    {
        return AssetManager::LoadMesh(ToMxString(path));
    }

    Resource<Mesh, ResourceFactory> AssetManager::LoadMesh(const MxString& path)
    {
        return ResourceFactory::Create<Mesh>(path);
    }

    Resource<Mesh, ResourceFactory> AssetManager::LoadMesh(const char* path)
    {
        return AssetManager::LoadMesh(MxString(path));
    }

    MxVector<Resource<Material, ResourceFactory>> AssetManager::LoadMaterials(StringId hash)
    {
        return AssetManager::LoadMaterials(FileManager::GetFilePath(hash));
    }

    MxVector<Resource<Material, ResourceFactory>> AssetManager::LoadMaterials(const FilePath& path)
    {
        return AssetManager::LoadMaterials(ToMxString(path));
    }

    MxVector<Resource<Material, ResourceFactory>> AssetManager::LoadMaterials(const MxString& path)
    {
        return MeshRenderer::LoadMaterials(path);
    }

    MxVector<Resource<Material, ResourceFactory>> AssetManager::LoadMaterials(const char* path)
    {
        return  AssetManager::LoadMaterials(MxString(path));
    }
}