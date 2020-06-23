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

#include <vector>
#include "Utilities/Math/Math.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Components/Transform.h"
#include "Utilities/AbstractFactory/AbstractFactory.h"
#include "Core/Resources/Mesh.h"

namespace MxEngine
{
	class MxInstanceImpl
	{
		Vector3 color{ 1.0f };
	public:
		Transform Transform;

		inline void SetColor(const Vector3& color)
		{
			this->color = Clamp(color, MakeVector3(0.0f), MakeVector3(1.0f));
		}

		inline const Vector3& GetColor() const
		{
			return this->color;
		}
	};

	class InstanceFactory
	{
		MAKE_COMPONENT(InstanceFactory);
	public:
		struct Factory
		{
			FactoryImpl<MxInstanceImpl> inner;

			template<typename T>
			void Destroy(LocalResource<T, Factory>& resource)
			{
				inner.GetPool<T>().Deallocate(resource.GetHandle());
			}

			template<typename T>
			auto& Get()
			{
				return inner.GetPool<T>();
			}
		};

		using ModelData = MxVector<Matrix4x4>;
		using NormalData = MxVector<Matrix3x3>;
		using ColorData = MxVector<Vector3>;
		using BufferIndex = size_t;

		using MxInstance = LocalResource<MxInstanceImpl, Factory>;
	private:
		Factory factory;
		ModelData models;
		NormalData normals;
		ColorData colors;
		BufferIndex bufferIndex = std::numeric_limits<BufferIndex>::max();

		template<typename T>
		BufferIndex AddInstancedBuffer(Mesh& mesh, const MxVector<T>& data)
		{
			auto VBO = GraphicFactory::Create<VertexBuffer>();
			auto VBL = GraphicFactory::Create<VertexBufferLayout>();
			VBL->Push<T>();
			VBO->Load((float*)data.data(), data.size() * sizeof(T) / sizeof(float), UsageType::DYNAMIC_DRAW);
			return mesh.AddInstancedBuffer(std::move(VBO), std::move(VBL));
		}

		template<typename T>
		void BufferDataByIndex(const Mesh& mesh, size_t index, const MxVector<T>& buffer)
		{
			auto VBO = mesh.GetBufferByIndex(index);
			VBO->BufferDataWithResize((float*)buffer.data(), buffer.size() * sizeof(T) / sizeof(float));
		}

		void RemoveInstancedBuffer(Mesh& mesh, size_t index);
		void Destroy();
	public:
		const auto& GetInstancePool() const { return this->factory.inner.Pool; }
		auto& GetInstancePool() { return this->factory.inner.Pool; };
		size_t GetCount() const { return this->GetInstancePool().Allocated(); }
		auto GetInstances() { return ComponentView{ this->GetInstancePool() }; }

		void Init();
		void SubmitInstances();
		MxInstance MakeInstance();

		ModelData& GetModelData();
		NormalData& GetNormalData();
		ColorData& GetColorData();

		InstanceFactory() = default;
		InstanceFactory(const InstanceFactory&) = delete;
		InstanceFactory(InstanceFactory&&) noexcept = default;
		InstanceFactory& operator=(const InstanceFactory&) = delete;
		InstanceFactory& operator=(InstanceFactory&&) noexcept = default;
		~InstanceFactory();
	};
}