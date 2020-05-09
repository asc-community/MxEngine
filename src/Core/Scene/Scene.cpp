// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"

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

    Scene::Scene(const std::string& name, const FilePath& directory)
        : name(name)
    {
        this->SetDirectory(directory);
    }

    const Scene::ObjectManager::Storage& Scene::GetObjectList() const
    {
        this->objectManager.Update();
        return this->objectManager.Get();
    }

    void Scene::PrepareRender()
    {
        for (auto& object : this->objectManager.Get())
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
        this->objectManager.Get().clear();
        this->resourceManager.Clear();
    }

    MxObject& Scene::CreateObject(const std::string& name, const FilePath& path)
    {
        MAKE_SCOPE_PROFILER("Scene::CreateObject");
        if (this->objectManager.Get().find(name) != this->objectManager.Get().end())
        {
            Logger::Instance().Warning("MxEngine::Scene", "overriding already existing object: " + name);
            this->DestroyObject(name);
        }
        // using Scene::LoadMesh -> path provided without scenePath
        auto ptr = MakeUnique<MxObject>(this->LoadMesh(name + "Mesh", path));
        auto& object = *ptr;
        this->objectManager.Add(name, std::move(ptr));
        return object;
    }

    MxObject& Scene::AddObject(const std::string& name, UniqueRef<MxObject> object)
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

    MxObject& Scene::CopyObject(const std::string& name, const std::string& existingObject)
    {
        auto& object = GetObject(existingObject);
        if (object.GetInstanceCount() > 0)
        {
            Logger::Instance().Error("MxEngine::Scene", "instanced objects cannot be copied: " + existingObject);
            return object;
        }
        return AddObject(name, MakeUnique<MxObject>(object.GetMesh()));
    }

    MxObject& Scene::GetObject(const std::string& name) const
    {
        this->objectManager.Update();
        if (this->objectManager.Get().find(name) == this->objectManager.Get().end())
        {
            static MxObject defaultObject;
            Logger::Instance().Warning("MxEngine::Scene", "object was not found: " + name);
            return defaultObject;
        }
        return *this->objectManager.Get()[name];
    }

    void Scene::DestroyObject(const std::string& name)
    {
        if (this->objectManager.Get().find(name) == this->objectManager.Get().end())
        {
            Logger::Instance().Warning("MxEngine::Scene", "trying to destroy object which not exists: " + name);
            return;
        }
        this->objectManager.Remove(name);
    }

    bool Scene::HasObject(const std::string& name) const
    {
        this->objectManager.Update();
        return this->objectManager.Exists(name);
    }

    Mesh* Scene::LoadMesh(const std::string& name, const FilePath& path)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadMesh");
        auto mesh = MakeUnique<Mesh>((this->scenePath / path).string());
        return this->GetResourceManager<Mesh>().Add(name, std::move(mesh));
    }

    Script* Scene::LoadScript(const std::string& name, const FilePath& path)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadScript");
        auto script = MakeUnique<Script>((this->scenePath / path).string());
        return this->GetResourceManager<Script>().Add(name, std::move(script));
    }

    Shader* Scene::LoadShader(const std::string& name, const FilePath& vertex, const FilePath& fragment)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadShader");
        auto shader = Graphics::Instance()->CreateShader(this->scenePath / vertex, this->scenePath / fragment);
        return this->GetResourceManager<Shader>().Add(name, std::move(shader));
    }

    Shader* Scene::LoadShader(const std::string& name, const FilePath& vertex, const FilePath& geometry, const FilePath& fragment)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadShader");
        auto shader = Graphics::Instance()->CreateShader(this->scenePath / vertex, this->scenePath / geometry, this->scenePath / fragment);
        return this->GetResourceManager<Shader>().Add(name, std::move(shader));
    }

    Texture* Scene::LoadTexture(const std::string& name, const FilePath& texture, TextureWrap wrap, bool genMipmaps, bool flipImage)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadTexture");
        auto textureObject = Graphics::Instance()->CreateTexture(this->scenePath / texture, wrap, genMipmaps, flipImage);
        return this->GetResourceManager<Texture>().Add(name, std::move(textureObject));
    }

    CubeMap* Scene::LoadCubeMap(const std::string& name, const FilePath& texture, bool genMipmaps, bool flipImage)
    {
        MAKE_SCOPE_PROFILER("Scene::LoadCubeMap");
        auto cubemapObject = Graphics::Instance()->CreateCubeMap(this->scenePath / texture, genMipmaps, flipImage);
        return this->GetResourceManager<CubeMap>().Add(name, std::move(cubemapObject));
    }

    void Scene::SetDirectory(const FilePath& path)
    {
        Logger::Instance().Debug("MxEngine::Scene", "setting " + this->name + " scene directory to: " + path.string());
        this->scenePath = path;
    }

    const FilePath& Scene::GetDirectory() const
    {
        return this->scenePath;
    }

    const std::string& Scene::GetName() const
    {
        return this->name;
    }
}