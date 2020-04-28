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

#include <vector>
#include "Utilities/Math/Math.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Components/Transform/Transform.h"

namespace MxEngine
{
	class MxInstance
	{
		inline static Vector3 zeroVec = MakeVector3(0.0f);
		Vector3 scaleFactor{ 1.0f };
		Vector4 color{ 1.0f };
	public:
		Transform Model;

		inline bool IsDrawn() const
		{
			return Model.GetScale() != zeroVec;
		}

		inline void SetColor(const Vector4& color)
		{
			this->color = Clamp(color, MakeVector4(0.0f), MakeVector4(1.0f));
		}

		inline const Vector4& GetColor() const
		{
			return this->color;
		}

		inline void Hide()
		{
			if (Model.GetScale() != zeroVec)
			{
				this->scaleFactor = Model.GetScale();
			}
			this->Model.SetScale(zeroVec);
		}

		inline void Show()
		{
			this->Model.SetScale(this->scaleFactor);
		}
	};

	template<typename T, typename = void>
	class Instancing;

	template<typename T>
	class MxInstanceWrapper
	{
		size_t Index;
		std::vector<MxInstance>* InstanceList;

		friend class Instancing<T>;
		friend typename T;
	public:
		MxInstance* operator->();
		const MxInstance* operator->() const;
	};

	template<typename T>
	class Instancing<T>
	{
	public:
		using InstanceList = std::vector<MxInstance>;
		using ModelData = std::vector<Matrix4x4>;
		using NormalData = std::vector<Matrix3x3>;
		using ColorData = std::vector<Vector4>;
	private:
		InstanceList instances;
		ModelData models;
		NormalData normals;
		ColorData colors;
	public:
		constexpr static size_t Undefined = std::numeric_limits<size_t>::max();
		size_t InstanceDataIndex = Undefined;

		const InstanceList& GetInstanceList() const;
		InstanceList& GetInstanceList();
		MxInstanceWrapper<T> MakeInstance();
		ModelData& GetModelData();
		NormalData& GetNormalData();
		ColorData& GetColorData();
		size_t GetCount() const;
	};

	template<typename T>
	inline const typename Instancing<T>::InstanceList& Instancing<T>::GetInstanceList() const
	{
		return this->instances;
	}

	template<typename T>
	inline typename Instancing<T>::InstanceList& Instancing<T>::GetInstanceList()
	{
		return this->instances;
	}

	template<typename T>
	inline MxInstanceWrapper<T> Instancing<T>::MakeInstance()
	{
		MxInstanceWrapper<T> wrapper;
		
		wrapper.Index = this->instances.size();
		wrapper.InstanceList = &this->instances;
		this->instances.emplace_back();

		return wrapper;
	}

	template<typename T>
	inline typename Instancing<T>::ModelData& Instancing<T>::GetModelData()
	{
		MAKE_SCOPE_PROFILER("Instancing::BufferModelData");

		this->models.resize(this->instances.size());
		auto model = this->models.begin();
		auto instance = this->instances.begin();
		for (; model != this->models.end(); model++, instance++)
		{
			instance->Model.GetMatrix(*model);
		}
		return this->models;
	}

	template<typename T>
	inline typename Instancing<T>::NormalData& Instancing<T>::GetNormalData()
	{
		MAKE_SCOPE_PROFILER("Instancing::BufferNormalData");

		this->normals.resize(this->models.size());
		auto normal = this->normals.begin();
		auto model = this->models.begin();
		auto instance = this->instances.begin();
		for (; model != this->models.end(); model++, normal++, instance++)
		{
			instance->Model.GetNormalMatrix(*model, *normal);
		}
		return this->normals;
	}

	template<typename T>
	inline typename Instancing<T>::ColorData& Instancing<T>::GetColorData()
	{
		MAKE_SCOPE_PROFILER("Instancing::BufferColorData");

		this->colors.resize(this->instances.size());
		auto instance = this->instances.begin();
		for (auto it = this->colors.begin(); it != this->colors.end(); it++, instance++)
		{
			*it = instance->GetColor();
		}
		return this->colors;
	}

	template<typename T>
	inline size_t Instancing<T>::GetCount() const
	{
		return this->instances.size();
	}

	template<typename T>
	inline MxInstance* MxInstanceWrapper<T>::operator->()
	{
		return this->InstanceList->data() + this->Index;
	}

	template<typename T>
	inline const MxInstance* MxInstanceWrapper<T>::operator->() const
	{
		return this->InstanceList->data() + this->Index;
	}
}