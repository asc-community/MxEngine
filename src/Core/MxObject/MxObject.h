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
#include "Core/Interfaces/IDrawable.h"
#include "Core/Interfaces/IMovable.h"
#include "Core/MxObject/Mesh.h"
#include "Core/Components/Transform.h"
#include "Core/Components/InstanceFactory.h"

namespace MxEngine
{
	class MxObject
	{		
		ComponentManager components;
		Vector3 forwardVec{ 0.0f, 0.0f, 1.0f }, upVec{ 0.0f, 1.0f, 0.0f }, rightVec{ 1.0f, 0.0f, 0.0f };
		Vector4 renderColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		bool shouldRender = true;
		bool instanceUpdate = true;
		mutable AABB boundingBox;

		void ReserveInstances(size_t count, UsageType usage);
	public:
		using Factory = AbstractFactoryImpl<MxObject>;
		using MxObjectHandle = Resource<MxObject, Factory>;

		static MxObjectHandle Create();
		static void Destroy(MxObjectHandle& object);

		template<typename T>
		static MxObject& GetByComponent(T& component)
		{
			auto& manager = component.GetParent();
			return *reinterpret_cast<MxObject*>((uint8_t*)&manager - offsetof(MxObject, components));
		}

		using ArrayBufferType = const float*;
		bool UseLOD = true;

		float TranslateSpeed = 1.0f;
		float RotateSpeed = 1.0f;
		float ScaleSpeed = 1.0f;
		GResource<Shader> ObjectShader;
		GResource<Texture> ObjectTexture;
		MxObject();
		MxObject(Mesh* mesh);
		MxObject(const MxObject&) = delete;
		MxObject(MxObject&&) = default;

		virtual void OnUpdate();
		virtual void OnRenderDraw();

		void SetMesh(Mesh* mesh);
		Mesh* GetMesh() const;
		void Hide();
		void Show();

		void SetForwardVector(const Vector3& forward);
		void SetUpVector(const Vector3& up);
		void SetRightVector(const Vector3& right);
		MxObject& Scale(float x, float y, float z);
		MxObject& Rotate(float x, float y, float z);
		void SetRenderColor(const Vector4& color);

		void AddInstancedBuffer(ArrayBufferType buffer, size_t count, size_t components, size_t perComponentFloats = 4, UsageType type = UsageType::DYNAMIC_DRAW);
		void BufferDataByIndex(size_t index, ArrayBufferType buffer, size_t count, size_t offset = 0);
		size_t GetBufferCount() const;
		InstanceFactory::MxInstance Instanciate();
		ComponentView<MxInstanceImpl> GetInstances() const;
		void MakeInstanced(size_t instanced, UsageType usage = UsageType::DYNAMIC_DRAW);
		void DestroyInstances();
		void SetAutoBuffering(bool value = true);
		void BufferInstances();

		const AABB& GetAABB() const;
		Transform& GetTransform();

		size_t GetIterator() const;
		bool IsLast(size_t iterator) const;
		size_t GetNext(size_t iterator) const;
		const SubMesh& GetCurrent(size_t iterator) const;
		const Transform& GetTransform() const;
		bool HasShader() const;
		const Vector4& GetRenderColor() const;
		const Shader& GetShader() const;
		bool IsDrawable() const;
		bool HasTexture() const;
		const Texture& GetTexture() const;
		size_t GetInstanceCount() const;

		MxObject& Translate(float x, float y, float z);
		MxObject& TranslateForward(float dist);
		MxObject& TranslateRight(float dist);
		MxObject& TranslateUp(float dist);
		MxObject& Rotate(float horz, float vert);
		const Vector3& GetForwardVector() const;
		const Vector3& GetUpVector() const;
		const Vector3& GetRightVector() const;

		template<typename T, typename... Args>
		auto AddComponent(Args&&... args)
		{
			return this->components.AddComponent<T>(std::forward<Args>(args)...);
		}

		template<typename T>
		auto GetComponent() const
		{
			return this->components.GetComponent<T>();
		}

		template<typename T>
		void RemoveComponent()
		{
			this->components.RemoveComponent<T>();
		}

		template<typename T>
		bool HasComponent() const
		{
			return this->components.HasComponent<T>();
		}
	};
}