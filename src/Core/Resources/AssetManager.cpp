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
    FilePath RegisterExternalFolder(const FilePath& filepath)
    {
        auto folder = filepath.parent_path();
        bool alreadyInWorkingDirectory = FileManager::IsInDirectory(folder, FileManager::GetWorkingDirectory());
        FileManager::RegisterExternalResource(folder);
        if (!alreadyInWorkingDirectory)
        {
            auto newpath = FileManager::GetProximatePath(filepath, folder.parent_path());
            return newpath;
        }
        else
        {
            return filepath;
        }
    }

    CubeMapHandle AssetManager::LoadCubeMap(StringId hash)
    {
        auto& path = FileManager::GetFilePath(hash);
        return GraphicFactory::Create<CubeMap>(path);
    }

    CubeMapHandle AssetManager::LoadCubeMap(const FilePath& path)
    {
        auto hash = FileManager::RegisterExternalResource(path);
        return AssetManager::LoadCubeMap(hash);
    }

    CubeMapHandle AssetManager::LoadCubeMap(const MxString& path)
    {
        return AssetManager::LoadCubeMap(ToFilePath(path));
    }

    CubeMapHandle AssetManager::LoadCubeMap(const char* path)
    {
        return AssetManager::LoadCubeMap(FilePath(path));
    }

    CubeMapHandle AssetManager::LoadCubeMap(StringId right, StringId left, StringId top, StringId bottom, StringId front, StringId back)
    {
        auto cubemap = GraphicFactory::Create<CubeMap>();

        cubemap->Load(
            FileManager::GetFilePath(right),
            FileManager::GetFilePath(left),
            FileManager::GetFilePath(top),
            FileManager::GetFilePath(bottom),
            FileManager::GetFilePath(front),
            FileManager::GetFilePath(back)
        );
        return cubemap;
    }

    CubeMapHandle AssetManager::LoadCubeMap(const FilePath& right, const FilePath& left, const FilePath& top, const FilePath& bottom, const FilePath& front, const FilePath& back)
    {
        std::array hashes = {
            FileManager::RegisterExternalResource(right),
            FileManager::RegisterExternalResource(left),
            FileManager::RegisterExternalResource(top),
            FileManager::RegisterExternalResource(bottom),
            FileManager::RegisterExternalResource(front),
            FileManager::RegisterExternalResource(back)
        };

        return AssetManager::LoadCubeMap(
            hashes[0], hashes[1], hashes[2], hashes[3], hashes[4], hashes[5]
        );
    }

    CubeMapHandle AssetManager::LoadCubeMap(const MxString& right, const MxString& left, const MxString& top, const MxString& bottom, const MxString& front, const MxString& back)
    {
        return AssetManager::LoadCubeMap(
            ToFilePath(right),  ToFilePath(left),  ToFilePath(top),
            ToFilePath(bottom), ToFilePath(front), ToFilePath(back)
        );
    }

    CubeMapHandle AssetManager::LoadCubeMap(const char* right, const char* left, const char* top, const char* bottom, const char* front, const char* back)
    {
        return AssetManager::LoadCubeMap(
            FilePath(right), FilePath(left), FilePath(top),
            FilePath(bottom), FilePath(front), FilePath(back)
        );
    }

    TextureHandle AssetManager::LoadTexture(StringId hash, TextureFormat format)
    {
        auto path = FileManager::GetFilePath(hash);
        return GraphicFactory::Create<Texture>(path,format);
    }

    TextureHandle AssetManager::LoadTexture(const FilePath& path, TextureFormat format)
    {
        auto hash = FileManager::RegisterExternalResource(path);
        return AssetManager::LoadTexture(hash, format);
    }

    TextureHandle AssetManager::LoadTexture(const MxString& path, TextureFormat format)
    {
        return AssetManager::LoadTexture(ToFilePath(path), format);
    }

    TextureHandle AssetManager::LoadTexture(const char* path, TextureFormat format)
    {
        return AssetManager::LoadTexture(FilePath(path), format);
    }

    ShaderHandle AssetManager::LoadShader(StringId vertex, StringId fragment)
    {
        auto shader = GraphicFactory::Create<Shader>();

        auto vp = FileManager::GetFilePath(vertex);
        auto fp = FileManager::GetFilePath(fragment);

        shader->Load(vp, fp);
        return shader;
    }

    ShaderHandle AssetManager::LoadShader(const FilePath& vertex, const FilePath& fragment)
    {
        auto vh = FileManager::RegisterExternalResource(vertex);
        auto fh = FileManager::RegisterExternalResource(fragment);
        return AssetManager::LoadShader(vh, fh);
    }

    ShaderHandle AssetManager::LoadShader(const MxString& vertex, const MxString& fragment)
    {
        return AssetManager::LoadShader(ToFilePath(vertex), ToFilePath(fragment));
    }

    ShaderHandle AssetManager::LoadShader(const char* vertex, const char* fragment)
    {
        return AssetManager::LoadShader(FilePath(vertex), FilePath(fragment));
    }

    ShaderHandle AssetManager::LoadShader(StringId vertex, StringId geometry, StringId fragment)
    {
        auto shader = GraphicFactory::Create<Shader>();

        auto vp = FileManager::GetFilePath(vertex);
        auto gp = FileManager::GetFilePath(geometry);
        auto fp = FileManager::GetFilePath(fragment);

        shader->Load(vp, gp, fp);
        return shader;
    }

    ShaderHandle AssetManager::LoadShader(const FilePath& vertex, const FilePath& geometry, const FilePath& fragment)
    {
        auto vh = FileManager::RegisterExternalResource(vertex);
        auto gh = FileManager::RegisterExternalResource(geometry);
        auto fh = FileManager::RegisterExternalResource(fragment);
        return AssetManager::LoadShader(vh, gh, fh);
    }

    ShaderHandle AssetManager::LoadShader(const MxString& vertex, const MxString& geometry, const MxString& fragment)
    {
        return AssetManager::LoadShader(ToFilePath(vertex), ToFilePath(geometry), ToFilePath(fragment));
    }

    ShaderHandle AssetManager::LoadShader(const char* vertex, const char* geometry, const char* fragment)
    {
        return AssetManager::LoadShader(FilePath(vertex), FilePath(geometry), FilePath(fragment));
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(StringId fragment)
    {
        // do not evaluating path, as screen-space shader loader forwards loading to shader loader
        return AssetManager::LoadScreenSpaceShader(FileManager::GetFilePath(fragment));
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(const FilePath& fragment)
    {
        return AssetManager::LoadShader(
            FileManager::GetEngineShaderDirectory() / "rect_vertex.glsl", fragment
        );
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(const MxString& fragment)
    {
        return AssetManager::LoadScreenSpaceShader(ToFilePath(fragment));
    }

    ShaderHandle AssetManager::LoadScreenSpaceShader(const char* fragment)
    {
        return AssetManager::LoadScreenSpaceShader(FilePath(fragment));
    }

    ComputeShaderHandle AssetManager::LoadComputeShader(StringId path)
    {
        auto shader = GraphicFactory::Create<ComputeShader>();
        auto filepath = FileManager::GetFilePath(path);
        shader->Load(filepath);
        return shader;
    }

    ComputeShaderHandle AssetManager::LoadComputeShader(const FilePath& path)
    {
        auto hash = FileManager::RegisterExternalResource(path);
        return AssetManager::LoadComputeShader(hash);
    }

    ComputeShaderHandle AssetManager::LoadComputeShader(const MxString& path)
    {
        return AssetManager::LoadComputeShader(ToFilePath(path));
    }

    ComputeShaderHandle AssetManager::LoadComputeShader(const char* path)
    {
        return AssetManager::LoadComputeShader(FilePath(path));
    }

    MeshHandle AssetManager::LoadMesh(StringId hash)
    {
        auto mesh = ResourceFactory::Create<Mesh>();
        auto& path = FileManager::GetFilePath(hash);
        mesh->Load(path);
        return mesh;
    }

    MeshHandle AssetManager::LoadMesh(const FilePath& path)
    {
        auto localPath = RegisterExternalFolder(path);
        auto hash = FileManager::RegisterExternalResource(localPath);
        return AssetManager::LoadMesh(hash);
    }

    MeshHandle AssetManager::LoadMesh(const MxString& path)
    {
        return AssetManager::LoadMesh(ToFilePath(path));
    }

    MeshHandle AssetManager::LoadMesh(const char* path)
    {
        return AssetManager::LoadMesh(FilePath(path));
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(StringId hash)
    {
        auto& path = FileManager::GetFilePath(hash);
        return MeshRenderer::LoadMaterials(path);
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(const FilePath& path)
    {
        auto hash = FileManager::RegisterExternalResource(path);
        return AssetManager::LoadMaterials(hash);
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(const MxString& path)
    {
        return AssetManager::LoadMaterials(ToFilePath(path));
    }

    MxVector<MaterialHandle> AssetManager::LoadMaterials(const char* path)
    {
        return AssetManager::LoadMaterials(FilePath(path));
    }

    AudioBufferHandle AssetManager::LoadAudio(StringId hash)
    {
        auto& path = FileManager::GetFilePath(hash);
        auto buffer = AudioFactory::Create<AudioBuffer>();
        buffer->Load(path);
        return buffer;
    }

    AudioBufferHandle AssetManager::LoadAudio(const FilePath& path)
    {
        auto hash = FileManager::RegisterExternalResource(path);
        return AssetManager::LoadAudio(hash);
    }

    AudioBufferHandle AssetManager::LoadAudio(const MxString& path)
    {
        return AssetManager::LoadAudio(ToFilePath(path));
    }

    AudioBufferHandle AssetManager::LoadAudio(const char* path)
    {
        return AssetManager::LoadAudio(FilePath(path));
    }
}