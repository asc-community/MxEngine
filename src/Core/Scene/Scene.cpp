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

#include "Scene.h"
#include "Utilities/Profiler/Profiler.h"
#include "Platform/GraphicAPI.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/String/String.h"

namespace MxEngine
{
    void Scene::OnLoad()
    {
        // is overriden in derived class
    }

    void Scene::OnUnload()
    {
        // is overriden in derived class
    }

    void Scene::OnCreate()
    {
        // is overriten in derived class
    }

    void Scene::OnDestroy()
    {
        // is overriten in derived class
    }

    void Scene::OnUpdate()
    {
        // is overriten in derived class
    }

    void Scene::OnRender()
    {
        // is overriten in derived class
    }

    Scene::Scene()
        : name("[[user_created]]")
    {
        
    }

    Scene::Scene(const MxString& name, const FilePath& directory)
        : name(name)
    {
        this->SetDirectory(directory);
    }

    const Scene::ObjectManager::Storage& Scene::GetObjectList() const
    {
        this->objectManager.Update();
        return this->objectManager.GetElements();
    }

    void Scene::PrepareRender()
    {
        for (auto& object : this->objectManager.GetElements())
        {
            object.second->OnRenderDraw();
        }
    }

    AppEventDispatcher& Scene::GetEventDispatcher()
    {
        return this->dispatcher;
    }

    void Scene::Clear()
    {
        this->objectManager.GetElements().clear();
        this->resourceManager.Clear();
    }

    MxObject& Scene::CreateObject(const MxString& name, const MxString& file)
    {
        MAKE_SCOPE_PROFILER("Scene::CreateObject");
        if (this->objectManager.GetElements().find(name) != this->objectManager.GetElements().end())
        {
            Logger::Instance().Warning("MxEngine::Scene", "overriding already existing object: " + name);
            this->DestroyObject(name);
        }
        // using Scene::LoadMesh -> path provided without scenePath
        auto ptr = MakeUnique<MxObject>(this->LoadMesh(file));
        auto& object = *ptr;
        this->objectManager.Add(name, std::move(ptr));
        return object;
    }

    MxObject& Scene::AddObject(const MxString& name, UniqueRef<MxObject> object)
    {
        if (this->objectManager.Exists(name))
        {
            Logger::Instance().Warning("MxEngine::Scene", "overriding already existing object: " + name);
            this->DestroyObject(name);
        }
        Logger::Instance().Debug("MxEngine::Scene", "adding object with name: " + name);
        auto& value = *object;
        this->objectManager.Add(name, std::move(object));
        return value;
    }

    MxObject& Scene::CopyObject(const MxString& name, const MxString& existingObject)
    {
        auto& object = GetObject(existingObject);
        if (object.GetInstanceCount() > 0)
        {
            Logger::Instance().Error("MxEngine::Scene", "instanced objects cannot be copied: " + existingObject);
            return object;
        }
        return AddObject(name, MakeUnique<MxObject>(object.GetMesh()));
    }

    MxObject& Scene::GetObject(const MxString& name) const
    {
        this->objectManager.Update();
        if (this->objectManager.GetElements().find(name) == this->objectManager.GetElements().end())
        {
            static MxObject defaultObject;
            Logger::Instance().Warning("MxEngine::Scene", "object was not found: " + name);
            return defaultObject;
        }
        return *this->objectManager.GetElements()[name];
    }

    void Scene::DestroyObject(const MxString& name)
    {
        if (this->objectManager.GetElements().find(name) == this->objectManager.GetElements().end())
        {
            Logger::Instance().Warning("MxEngine::Scene", "trying to destroy object which not exists: " + name);
            return;
        }
        this->objectManager.Remove(name);
    }

    bool Scene::HasObject(const MxString& name) const
    {
        this->objectManager.Update();
        return this->objectManager.Exists(name);
    }

    Mesh* Scene::LoadMesh(const MxString& name)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadMesh");
        auto mesh = MakeUnique<Mesh>(ToMxString(FileModule::GetFilePath(MakeStringId(name))));
        return this->GetResourceManager<Mesh>().Add(name, std::move(mesh));
    }

    Script* Scene::LoadScript(const MxString& name)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadScript");
        auto script = MakeUnique<Script>(FileModule::GetFilePath(MakeStringId(name)));
        return this->GetResourceManager<Script>().Add(name, std::move(script));
    }

    Shader* Scene::LoadShader(const MxString& name, const MxString& vertex, const MxString& fragment)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadShader");
        auto shader = MakeUnique<Shader>(
            ToMxString(FileModule::GetFilePath(MakeStringId(vertex))),
            ToMxString(FileModule::GetFilePath(MakeStringId(fragment)))
            );
        return this->GetResourceManager<Shader>().Add(name, std::move(shader));
    }

    Shader* Scene::LoadShader(const MxString& name, const MxString& vertex, const MxString& geometry, const MxString& fragment)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadShader");
        auto shader = MakeUnique<Shader>(
            ToMxString(FileModule::GetFilePath(MakeStringId(vertex))),
            ToMxString(FileModule::GetFilePath(MakeStringId(geometry))),
            ToMxString(FileModule::GetFilePath(MakeStringId(fragment)))
        );
        return this->GetResourceManager<Shader>().Add(name, std::move(shader));
    }

    Texture* Scene::LoadTexture(const MxString& name, TextureWrap wrap, bool genMipmaps, bool flipImage)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadTexture");
        auto textureObject = MakeUnique<Texture>(ToMxString(FileModule::GetFilePath(MakeStringId(name))), wrap, genMipmaps, flipImage);
        return this->GetResourceManager<Texture>().Add(name, std::move(textureObject));
    }

    CubeMap* Scene::LoadCubeMap(const MxString& name, bool genMipmaps, bool flipImage)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadCubeMap");
        auto cubemapObject = MakeUnique<CubeMap>(ToMxString(FileModule::GetFilePath(MakeStringId(name))), genMipmaps, flipImage);
        return this->GetResourceManager<CubeMap>().Add(name, std::move(cubemapObject));
    }

    void Scene::SetDirectory(const FilePath& path)
    {
        Logger::Instance().Debug("MxEngine::Scene", "setting " + this->name + " scene directory to: " + ToMxString(path));
        this->scenePath = path;
    }

    const FilePath& Scene::GetDirectory() const
    {
        return this->scenePath;
    }

    const MxString& Scene::GetName() const
    {
        return this->name;
    }
}