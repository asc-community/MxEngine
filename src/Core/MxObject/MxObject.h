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
#include "Core/Interfaces/IDrawable.h"
#include "Core/Interfaces/IMovable.h"
#include "Core/MxObject/RenderObject.h"
#include "Core/Components/Transform/Transform.h"

namespace MxEngine
{
	class MxObject : public IDrawable, public IMovable
	{
		size_t instanceCount = 0;
		Vector3 forwardVec{ 0.0f, 0.0f, 1.0f }, upVec{ 0.0f, 1.0f, 0.0f }, rightVec{ 1.0f, 0.0f, 0.0f };
		bool shouldRender = true;
	protected:
		Mesh* ObjectMesh = nullptr;
	public:
		using ArrayBufferType = const float*;

		Transform ObjectTransform;
		Shader* ObjectShader = nullptr;
		Texture* ObjectTexture = nullptr;
		MxObject() = default;
		MxObject(Mesh* mesh);
		MxObject(const MxObject&) = delete;
		MxObject(MxObject&&) = default;

		virtual void OnUpdate();

		void SetMesh(Mesh* mesh);
		Mesh& GetMesh();
		const Mesh& GetMesh() const;
		void Hide();
		void Show();

		void SetForwardVector(const Vector3& forward);
		void SetUpVector(const Vector3& up);
		void SetRightVector(const Vector3& right);
		MxObject& Scale(float x, float y, float z);
		MxObject& Rotate(float x, float y, float z);

		void AddInstancedBuffer(ArrayBufferType buffer, size_t count, size_t components, UsageType type = UsageType::STATIC_DRAW);
		void BufferDataByIndex(size_t index, ArrayBufferType buffer, size_t count, size_t offset = 0);
		template<typename T>
		void GenerateDataByIndex(size_t index, T&& generator);
		size_t GetBufferCount() const;

		// Inherited via IDrawable
		virtual size_t GetIterator() const override;
		virtual bool IsLast(size_t iterator) const override;
		virtual size_t GetNext(size_t iterator) const override;
		virtual const IRenderable& GetCurrent(size_t iterator) const override;
		virtual const Matrix4x4& GetModelMatrix() const override;
		virtual bool HasShader() const override;
		virtual const Shader& GetShader() const override;
		virtual bool IsDrawable() const override;
		virtual bool HasTexture() const override;
		virtual const Texture& GetTexture() const override;
		virtual size_t GetInstanceCount() const override;

		// Inherited via IMovable
		virtual MxObject& Translate(float x, float y, float z) override;
		virtual MxObject& TranslateForward(float dist) override;
		virtual MxObject& TranslateRight(float dist) override;
		virtual MxObject& TranslateUp(float dist) override;
		virtual MxObject& Rotate(float horz, float vert) override;
		virtual const Vector3& GetForwardVector() const override;
		virtual const Vector3& GetUpVector() const override;
		virtual const Vector3& GetRightVector() const override;
	};

	template<typename T>
	inline void MxObject::GenerateDataByIndex(size_t index, T&& generator)
	{
		using DataType = decltype(generator(0));
		static std::vector<float> buffer;

		size_t instanceSize = sizeof(DataType);
		size_t componentCount = instanceSize / sizeof(float);
		buffer.resize(this->instanceCount * componentCount);

		// fills float array with data from generator function
		for (size_t i = 0; i < this->instanceCount; i++)
		{
			auto* data = reinterpret_cast<DataType*>(buffer.data() + i * componentCount);
			*data = generator(static_cast<int>(i));
		}

		this->BufferDataByIndex(index, buffer.data(), buffer.size());
	}
}