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
#include "Core/Object/RenderObject.h"

namespace MxEngine
{
	class MxObject : public IDrawable, public IMovable
	{
		mutable Matrix4x4 Model;
		Vector3 translation{ 0.0f };
		Vector3 forwardVec{ 0.0f, 0.0f, 1.0f }, upVec{ 0.0f, 1.0f, 0.0f }, rightVec{ 1.0f, 0.0f, 0.0f };
		Quaternion rotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		Vector3 scale{ 1.0f };
		size_t instanceCount = 0;
		mutable bool needUpdate = true;
		bool shouldRender = true;
	protected:
		Ref<RenderObjectContainer> object;
	public:
		using ArrayBufferType = std::vector<float>;

		Ref<Shader> Shader;
		Ref<Texture> Texture;
		MxObject() = default;
		MxObject(const Ref<RenderObjectContainer>& object);
		MxObject(const MxObject&) = delete;
		MxObject(MxObject&&) = default;

		virtual void OnUpdate();

		void Load(const Ref<RenderObjectContainer>& object);
		Ref<RenderObjectContainer>& GetObjectBase();
		const Ref<RenderObjectContainer>& GetObjectBase() const;
		void Hide();
		void Show();

		const Vector3& GetTranslation() const;
		const Quaternion& GetRotation() const;
		const Vector3& GetScale() const;

		MxObject& Scale(float scale);
		MxObject& Scale(float scaleX, float scaleY, float scaleZ);
		MxObject& Scale(const Vector3& scale);

		MxObject& Rotate(float angle, const Vector3& rotate);
		MxObject& RotateX(float angle);
		MxObject& RotateY(float angle);
		MxObject& RotateZ(float angle);

		MxObject& Translate(const Vector3& dist);
		MxObject& TranslateX(float x);
		MxObject& TranslateY(float y);
		MxObject& TranslateZ(float z);

		template<typename T>
		void AddInstanceBufferGenerator(T&& generator, size_t count, UsageType type = UsageType::STATIC_DRAW);
		void AddInstanceBuffer(const ArrayBufferType& buffer, size_t count, UsageType type = UsageType::STATIC_DRAW);

		// Inherited via IDrawable
		virtual size_t GetIterator() const override;
		virtual bool IsLast(size_t iterator) const override;
		virtual size_t GetNext(size_t iterator) const override;
		virtual const IRenderable& GetCurrent(size_t iterator) const override;
		virtual const Matrix4x4& GetModel() const override;
		virtual bool HasShader() const override;
		virtual const MxEngine::Shader& GetShader() const override;
		virtual bool IsDrawable() const override;
		virtual bool HasTexture() const override;
		virtual const MxEngine::Texture& GetTexture() const override;
		virtual size_t GetInstanceCount() const override;

		// Inherited via IMovable
		virtual MxObject& Translate(float x, float y, float z) override;
		virtual MxObject& TranslateForward(float dist) override;
		virtual MxObject& TranslateRight(float dist) override;
		virtual MxObject& TranslateUp(float dist) override;
		virtual MxObject& Rotate(float horz, float vert) override;
		virtual void SetForwardVector(const Vector3& forward) override;
		virtual void SetUpVector(const Vector3& up) override;
		virtual void SetRightVector(const Vector3& right) override;
		virtual const Vector3& GetForwardVector() const override;
		virtual const Vector3& GetUpVector() const override;
		virtual const Vector3& GetRightVector() const override;
	};

	template<typename T>
	inline void MxObject::AddInstanceBufferGenerator(T&& generator, size_t count, UsageType type)
	{
		using DataType = decltype(generator(0));

		size_t instanceSize = sizeof(DataType);
		size_t componentCount = instanceSize / sizeof(float);
		ArrayBufferType buffer(count * componentCount);

		// fills float array with data from generator functions
		for (size_t i = 0; i < count; i++)
		{
			auto* data = reinterpret_cast<DataType*>(buffer.data() + i * componentCount);
			*data = generator(i);
		}

		this->AddInstanceBuffer(buffer, componentCount, type);
	}
}