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
}