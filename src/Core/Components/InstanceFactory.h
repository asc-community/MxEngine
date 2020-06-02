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

namespace MxEngine
{
	class MxInstanceImpl
	{
		Vector4 color{ 1.0f };
	public:
		Transform Model;

		inline void SetColor(const Vector4& color)
		{
			this->color = Clamp(color, MakeVector4(0.0f), MakeVector4(1.0f));
		}

		inline const Vector4& GetColor() const
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
		using ColorData = MxVector<Vector4>;

		using MxInstance = LocalResource<MxInstanceImpl, Factory>;
	private:
		Factory factory;
		ModelData models;
		NormalData normals;
		ColorData colors;
	public:
		constexpr static size_t Undefined = std::numeric_limits<size_t>::max();
		size_t InstanceDataIndex = Undefined;

		const auto& GetInstancePool() const { return this->factory.inner.Pool; }
		auto& GetInstancePool() { return this->factory.inner.Pool; };
		size_t GetCount() const { return this->GetInstancePool().Allocated(); }

		auto MakeInstance()
		{

			auto uuid = UUIDGenerator::Get();
			size_t index = this->GetInstancePool().Allocate(uuid);
			auto instance = MxInstance(uuid, index, &this->factory);
			return std::move(instance);
		}

		ModelData& GetModelData()
		{
			MAKE_SCOPE_PROFILER("Instancing::BufferModelData");

			this->models.resize(this->GetInstancePool().Allocated());
			auto model = this->models.begin();
			auto instance = this->GetInstancePool().begin();
			for (; model != this->models.end(); model++, instance++)
			{
				instance->value.Model.GetMatrix(*model);
			}
			return this->models;
		}

		NormalData& GetNormalData()
		{
			MAKE_SCOPE_PROFILER("Instancing::BufferNormalData");

			this->normals.resize(this->GetInstancePool().Allocated());
			auto normal = this->normals.begin();
			auto model = this->models.begin();
			auto instance = this->GetInstancePool().begin();
			for (; model != this->models.end(); model++, normal++, instance++)
			{
				instance->value.Model.GetNormalMatrix(*model, *normal);
			}
			return this->normals;
		}

		ColorData& GetColorData()
		{
			MAKE_SCOPE_PROFILER("Instancing::BufferColorData");

			this->colors.resize(this->GetInstancePool().Allocated());
			auto instance = this->GetInstancePool().begin();
			for (auto it = this->colors.begin(); it != this->colors.end(); it++, instance++)
			{
				*it = instance->value.GetColor();
			}
			return this->colors;
		}
	};
}