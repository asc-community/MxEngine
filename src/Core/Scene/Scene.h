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

#pragma once

#include "Utilities/GenericStorage/GenericStorage.h"
#include "Utilities/ResourceStorage/ResourceStorage.h"
#include "Utilities/LifetimeManager/LifetimeManager.h"
#include "Utilities/Array/ArrayView.h"

#include "Core/Camera/CameraController.h"
#include "Core/Lighting/DirectionalLight/DirectionalLight.h"
#include "Core/Lighting/PointLight/PointLight.h"
#include "Core/Lighting/SpotLight/SpotLight.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Interfaces/IEvent.h"
#include "Core/Skybox/Skybox.h"

#include "Library/Scripting/Script/Script.h"

namespace MxEngine
{
    class Application;

    class Scene
    {
    public:
        using ResourceManager = MultiStorage<ResourceStorage, Texture, CubeMap, Mesh, Shader, Script, FrameBuffer>;
        using ObjectManager = LifetimeManager<UniqueRef<MxObject>>;
        template<typename LightType>
        class LightContainer
        {
            using Storage = std::vector<LightType>;
            Storage storage;
        public:
            size_t GetCount() const;
            void SetCount(size_t count);
            LightType& operator[](size_t index);
            const LightType& operator[](size_t index) const;
            ArrayView<LightType> GetView();
        };
    private:
        MxString name;
        FilePath scenePath;
        ResourceManager resourceManager;
        mutable ObjectManager objectManager;
        AppEventDispatcher dispatcher;
    public:
        Scene();
        Scene(const MxString& name, const FilePath& directory);
        Scene(Scene&&) = default;
        Scene& operator=(Scene&&) = default;
        virtual ~Scene() = default;

        CameraController Viewport;
        UniqueRef<Skybox> SceneSkybox;
        DirectionalLight GlobalLight;
        LightContainer<PointLight> PointLights;
        LightContainer<SpotLight> SpotLights;

        virtual void OnLoad();
        virtual void OnUnload();
        virtual void OnCreate();
        virtual void OnDestroy();
        virtual void OnUpdate();
        virtual void OnRender();

        template<typename Resource>
        ResourceManager::ConcreteStorage<Resource>& GetResourceManager();
        template<typename Resource>
        Resource* GetResource(const MxString& name);
        const ObjectManager::Storage& GetObjectList() const;
        void PrepareRender();
        AppEventDispatcher& GetEventDispatcher();
        void Clear();

        MxObject& CreateObject(const MxString& name, const MxString& filename);
        MxObject& AddObject(const MxString& name, UniqueRef<MxObject> object);
        MxObject& CopyObject(const MxString& name, const MxString& existingObject);
        MxObject& GetObject(const MxString& name) const;
        void DestroyObject(const MxString& name);
        bool HasObject(const MxString& name) const;

        Mesh* LoadMesh(const MxString& name);
        Script* LoadScript(const MxString& name);
        Shader* LoadShader(const MxString& name, const MxString& vertex, const MxString& fragment);
        Shader* LoadShader(const MxString& name, const MxString& vertex, const MxString& geometry, const MxString& fragment);
        Texture* LoadTexture(const MxString& name, TextureWrap wrap = TextureWrap::REPEAT, bool genMipmaps = true, bool flipImage = true);
        CubeMap* LoadCubeMap(const MxString& name, bool genMipmaps = true, bool flipImage = false);
    
        void SetDirectory(const FilePath& path);
        const FilePath& GetDirectory() const;
        const MxString& GetName() const;
    };

    template<typename Resource>
    inline Scene::ResourceManager::ConcreteStorage<Resource>& Scene::GetResourceManager()
    {
        return this->resourceManager.Get<Resource>();
    }

    template<typename Resource>
    inline Resource* Scene::GetResource(const MxString& name)
    {
        MX_ASSERT(this->GetResourceManager<Resource>().Exists(name));
        return this->GetResourceManager<Resource>().Get(name);
    }

    template<typename LightType>
    inline size_t Scene::LightContainer<LightType>::GetCount() const
    {
        return storage.size();
    }

    template<typename LightType>
    inline void Scene::LightContainer<LightType>::SetCount(size_t count)
    {
        storage.resize(count);
    }

    template<typename LightType>
    inline LightType& Scene::LightContainer<LightType>::operator[](size_t index)
    {
        MX_ASSERT(index < storage.size());
        return storage[index];
    }

    template<typename LightType>
    inline const LightType& Scene::LightContainer<LightType>::operator[](size_t index) const
    {
        MX_ASSERT(index < storage.size());
        return storage[index];
    }

    template<typename LightType>
    inline ArrayView<LightType> Scene::LightContainer<LightType>::GetView()
    {
        return ArrayView<LightType>(this->storage);
    }
}