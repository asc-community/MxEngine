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

#include "AssetManager.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Core/Components/Rendering/MeshRenderer.h"

namespace MxEngine
{
    CubeMapHandle AssetManager::LoadCubeMap(StringId hash)
    {
        return AssetManager::LoadCubeMap(FileManager::GetFilePath(hash));
    }

    CubeMapHandle AssetManager::LoadCubeMap(const FilePath& path)
    {
        return AssetManager::LoadCubeMap(ToMxString(path));
    }

    CubeMapHandle AssetManager::LoadCubeMap(const MxString& path)
    {
        return GraphicFactory::Create<CubeMap>(path);
    }

    CubeMapHandle AssetManager::LoadCubeMap(const char* path)
    {
        return AssetManager::LoadCubeMap(MxString(path));
    }

    CubeMapHandle AssetManager::LoadCubeMap(StringId right, StringId left, StringId top, StringId bottom, StringId front, StringId back)
    {
        return AssetManager::LoadCubeMap(
            FileManager::GetFilePath(right), FileManager::GetFilePath(left),
            FileManager::GetFilePath(top),   FileManager::GetFilePath(bottom),
            FileManager::GetFilePath(front), FileManager::GetFilePath(back)
        );
    }

    CubeMapHandle AssetManager::LoadCubeMap(const FilePath& right, const FilePath& left, const FilePath& top, const FilePath& bottom, const FilePath& front, const FilePath& back)
    {
        return AssetManager::LoadCubeMap(
            ToMxString(right),  ToMxString(left),  ToMxString(top),
            ToMxString(bottom), ToMxString(front), ToMxString(back)
        );
    }

    CubeMapHandle AssetManager::LoadCubeMap(const MxString& right, const MxString& left, const MxString& top, const MxString& bottom, const MxString& front, const MxString& back)
    {
        auto cubemap = GraphicFactory::Create<CubeMap>();
        cubemap->Load(right, left, top, bottom, front, back);
        return cubemap;
    }

    CubeMapHandle AssetManager::LoadCubeMap(const char* right, const char* left, const char* top, const char* bottom, const char* front, const char* back)
    {
        return AssetManager::LoadCubeMap(
            MxString(right),  MxString(left), MxString(top),
            MxString(bottom), MxString(front), MxString(back)
        );
    }

    TextureHandle AssetManager::LoadTexture(StringId hash)
    {
        return AssetManager::LoadTexture(FileManager::GetFilePath(hash));
    }

    TextureHandle AssetManager::LoadTexture(const FilePath& path)
    {
        return AssetManager::LoadTexture(ToMxString(path));
    }

    TextureHandle AssetManager::LoadTexture(const MxString& path)
    {
        return GraphicFactory::Create<Texture>(path);
    }

    TextureHandle AssetManager::LoadTexture(const char* path)
    {
        return AssetManager::LoadTexture(MxString(path));
    }

    ShaderHandle AssetManager::LoadShader(StringId vertex, StringId fragment)
    {
        return AssetManager::LoadShader(FileManager::GetFilePath(vertex), FileManager::GetFilePath(fragment));
    }

    ShaderHandle AssetManager::LoadShader(const FilePath& vertex, const FilePath& fragment)
    {
        return AssetManager::LoadShader(ToMxString(vertex), ToMxString(fragment));
    }

    ShaderHandle AssetManager::LoadShader(const MxString& vertex, const MxString& fragment)
    {
        return GraphicFactory::Create<Shader>(vertex, fragment);
    }

    ShaderHandle AssetManager::LoadShader(const char* vertex, const char* fragment)
    {
        return AssetManager::LoadShader(MxString(vertex), MxString(fragment));
    }

    ShaderHandle AssetManager::LoadShader(StringId vertex, StringId geometry, StringId fragment)
    {
        return AssetManager::LoadShader(FileManager::GetFilePath(vertex), FileManager::GetFilePath(geometry), FileManager::GetFilePath(fragment));
    }

    ShaderHandle AssetManager::LoadShader(const FilePath& vertex, const FilePath& geometry, const FilePath& fragment)
    {
        return AssetManager::LoadShader(ToMxString(vertex), ToMxString(geometry), ToMxString(fragment));
    }

    ShaderHandle AssetManager::LoadShader(const MxString& vertex, const MxString& geometry, const MxString& fragment)
    {
        return GraphicFactory::Create<Shader>(vertex, geometry, fragment);
    }

    ShaderHandle AssetManager::LoadShader(const char* vertex, const char* geometry, const char* fragment)
    {
        return AssetManager::LoadShader(MxString(vertex), MxString(geometry), MxString(fragment));
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(StringId fragment)
    {
        return AssetManager::LoadScreenSpaceShader(FileManager::GetFilePath(fragment));
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(const FilePath& fragment)
    {
        return AssetManager::LoadScreenSpaceShader(ToMxString(fragment));
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(const MxString& fragment)
    {
        auto shader = GraphicFactory::Create<Shader>();
        shader->LoadFromString(
            #include "Platform/OpenGL/Shaders/rect_vertex.glsl"
            ,
            File(fragment).ReadAllText()
        );
        return shader;
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(const char* fragment)
    {
        return AssetManager::LoadScreenSpaceShader(MxString(fragment));
    }

    MeshHandle AssetManager::LoadMesh(StringId hash)
    {
        return AssetManager::LoadMesh(FileManager::GetFilePath(hash));
    }

    MeshHandle AssetManager::LoadMesh(const FilePath& path)
    {
        return AssetManager::LoadMesh(ToMxString(path));
    }

    MeshHandle AssetManager::LoadMesh(const MxString& path)
    {
        return ResourceFactory::Create<Mesh>(path);
    }

    MeshHandle AssetManager::LoadMesh(const char* path)
    {
        return AssetManager::LoadMesh(MxString(path));
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(StringId hash)
    {
        return AssetManager::LoadMaterials(FileManager::GetFilePath(hash));
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(const FilePath& path)
    {
        return AssetManager::LoadMaterials(ToMxString(path));
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(const MxString& path)
    {
        return MeshRenderer::LoadMaterials(path);
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(const char* path)
    {
        return AssetManager::LoadMaterials(MxString(path));
    }

    AudioBufferHandle AssetManager::LoadAudio(StringId hash)
    {
        return AssetManager::LoadAudio(FileManager::GetFilePath(hash));
    }

    AudioBufferHandle AssetManager::LoadAudio(const FilePath& filepath)
    {
        return AssetManager::LoadAudio(ToMxString(filepath));
    }

    AudioBufferHandle AssetManager::LoadAudio(const MxString& filepath)
    {
        auto buffer = AudioFactory::Create<AudioBuffer>();
        buffer->Load(filepath);
        return buffer;
    }

    AudioBufferHandle AssetManager::LoadAudio(const char* filepath)
    {
        return AssetManager::LoadAudio(MxString(filepath));
    }
}