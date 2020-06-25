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

#include "Core/Resources/Mesh.h"
#include "Core/Components/Transform.h"
#include "Core/Components/InstanceFactory.h"
#include "Core/Components/Rendering/MeshRenderer.h"

GENERATE_METHOD_CHECK(Init, Init())

namespace MxEngine
{
	class MxObject
	{
		mutable AABB boundingBox;

		using EngineHandle = size_t;
		constexpr static EngineHandle InvalidHandle = std::numeric_limits<EngineHandle>::max();
		EngineHandle handle = InvalidHandle;

	public:
		MxString Name = UUIDGenerator::Get();
		float TranslateSpeed = 1.0f;
		float RotateSpeed = 1.0f;
		float ScaleSpeed = 1.0f;
		Transform::Handle Transform;
	private:
		ComponentManager components;
	public:
		using Factory = AbstractFactoryImpl<MxObject>;
		using Handle = Resource<MxObject, Factory>;

		static Handle Create();
		static void Destroy(Handle& object);
		static void Destroy(MxObject& object);

		static ComponentView<MxObject> GetObjects();
		static Handle GetByName(const MxString& name);

		template<typename T>
		static MxObject& GetByComponent(T& component)
		{
			auto handle = reinterpret_cast<EngineHandle>(component.UserData);
			MX_ASSERT(handle != InvalidHandle);
			auto& managedObject = Factory::Get<MxObject>()[handle];
			return managedObject.value;
		}

		template<typename T>
		static Handle GetHandleByComponent(T& component)
		{
			auto handle = reinterpret_cast<EngineHandle>(component.UserData);
			MX_ASSERT(handle != InvalidHandle);
			auto& managedObject = Factory::Get<MxObject>()[handle];
			MX_ASSERT(managedObject.refCount > 0 && managedObject.uuid != UUIDGenerator::GetNull());
			return MxObjectHandle(managedObject.uuid, handle);
		}

		MxObject();
		MxObject(const MxObject&) = delete;
		MxObject& operator=(const MxObject&) = delete;
		MxObject(MxObject&&) = default;
		MxObject& operator=(MxObject&&) = default;
		~MxObject();

		template<typename T, typename... Args>
		auto AddComponent(Args&&... args)
		{
			auto component = this->components.AddComponent<T>(std::forward<Args>(args)...);
			component->UserData = reinterpret_cast<void*>(this->handle);
			if constexpr (has_method_Init<T>::value) component->Init();
			return component;
		}

		template<typename T>
		auto GetComponent() const
		{
			return this->components.GetComponent<T>();
		}

		template<typename T>
		auto GetOrAddComponent()
		{
			if (!this->HasComponent<T>())
				return this->AddComponent<T>();
			else
				return this->GetComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			static_assert(!std::is_same_v<T, MxEngine::Transform>, "Transform component cannot be deleted");
			this->components.RemoveComponent<T>();
		}

		template<typename T>
		bool HasComponent() const
		{
			return this->components.HasComponent<T>();
		}
	};
}