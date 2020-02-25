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

#include "Object.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Utilities/Profiler/Profiler.h"



namespace MxEngine
{
	void MxObject::AddInstanceBuffer(const ArrayBufferType& buffer, size_t count, UsageType type)
	{
		if (this->object == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "trying to add buffer to not existing object");
			return;
		}

		size_t objectCount = buffer.size() / count;

		if (this->instanceCount == 0) this->instanceCount = objectCount;
		if (this->instanceCount > objectCount)
		{
			Logger::Instance().Error("MxEngine::MxObject", "instance buffer was not added as it contains not enough information");
			return;
		}
		else if (this->instanceCount < objectCount)
		{
			Logger::Instance().Error("MxEngine::MxObject", "instance buffer contains more data than required, additional will be ignored");
		}

		for (auto& object : this->object->GetRenderObjects())
		{
			auto VBO = Graphics::Instance()->CreateVertexBuffer(buffer, type);
			auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
			VBL->PushFloat(count);
			object.AddInstancedBuffer(std::move(VBO), std::move(VBL));
		}
	}

	MxObject::MxObject(const Ref<RenderObjectContainer>& object)
	{
		Load(object);
	}

	void MxObject::OnUpdate()
	{
		// this method is overloaded in derived class
	}

	void MxObject::Load(const Ref<RenderObjectContainer>& object)
	{
		this->object = object;
	}

	Ref<RenderObjectContainer>& MxObject::GetObjectBase()
	{
		return this->object;
	}

	const Ref<RenderObjectContainer>& MxObject::GetObjectBase() const
	{
		return this->object;
	}

	void MxObject::Hide()
	{
		this->shouldRender = false;
	}

	void MxObject::Show()
	{
		this->shouldRender = true;
	}

	const Vector3& MxObject::GetTranslation() const
	{
		return this->translation;
	}

	const Quaternion& MxObject::GetRotation() const
	{
		return this->rotation;
	}

	const Vector3& MxObject::GetScale() const
	{
		return this->scale;
	}

	MxObject& MxObject::Scale(float scale)
	{
		return Scale(Vector3(scale));
	}

	MxObject& MxObject::Scale(float scaleX, float scaleY, float scaleZ)
	{
		return Scale(Vector3(scaleX, scaleY, scaleZ));
	}

	MxObject& MxObject::Scale(const Vector3& scale)
	{
		needUpdate = true;
		this->scale.x *= scale.x;
		this->scale.y *= scale.y;
		this->scale.z *= scale.z;
		return *this;
	}

	MxObject& MxObject::Rotate(float angle, const Vector3& vec)
	{
		needUpdate = true;
		this->rotation *= MakeQuaternion(Radians(angle), vec);
		return *this;
	}

	MxObject& MxObject::RotateX(float angle)
	{
		return Rotate(angle, Vector3(1.0f, 0.0f, 0.0f));
	}

	MxObject& MxObject::RotateY(float angle)
	{
		return Rotate(angle, Vector3(0.0f, 1.0f, 0.0f));
	}

	MxObject& MxObject::RotateZ(float angle)
	{
		return Rotate(angle, Vector3(0.0f, 0.0f, 1.0f));
	}

	MxObject& MxObject::Translate(float x, float y, float z)
	{
		needUpdate = true;
		this->translation += Vector3(x, y, z);
		return *this;
	}

	MxObject& MxObject::TranslateForward(float dist)
	{
		this->Translate(this->rotation * this->forwardVec * dist);
		return *this;
	}

	MxObject& MxObject::TranslateRight(float dist)
	{
		this->Translate(this->rotation * this->rightVec * dist);
		return *this;
	}

	MxObject& MxObject::TranslateUp(float dist)
	{
		this->Translate(this->rotation * this->upVec * dist);
		return *this;
	}

	MxObject& MxObject::Rotate(float horz, float vert)
	{
		Rotate(horz, this->rotation * this->upVec);
		Rotate(vert, this->rotation * this->rightVec);
		
		return *this;
	}

	void MxObject::SetForwardVector(const Vector3& forward)
	{
		this->forwardVec = forward;
	}

	void MxObject::SetUpVector(const Vector3& up)
	{
		this->upVec = up;
	}

	void MxObject::SetRightVector(const Vector3& right)
	{
		this->rightVec = right;
	}

	const Vector3& MxObject::GetForwardVector() const
	{
		return this->forwardVec;
	}

	const Vector3& MxObject::GetUpVector() const
	{
		return this->upVec;
	}

	const Vector3& MxObject::GetRightVector() const
	{
		return this->rightVec;
	}

	MxObject& MxObject::Translate(const Vector3& dist)
	{
		return this->Translate(dist.x, dist.y, dist.z);
	}

	MxObject& MxObject::TranslateX(float x)
	{
		return Translate(x, 0.0f, 0.0f);
	}

	MxObject& MxObject::TranslateY(float y)
	{
		return Translate(0.0f, y, 0.0f);
	}

	MxObject& MxObject::TranslateZ(float z)
	{
		return Translate(0.0f, 0.0f, z);
	}

	size_t MxObject::GetIterator() const
	{
		return 0;
	}

	bool MxObject::IsLast(size_t iterator) const
	{
		return this->GetObjectBase()->GetRenderObjects().size() == iterator;
	}

	size_t MxObject::GetNext(size_t iterator) const
	{
		return iterator + 1;
	}

	const IRenderable& MxObject::GetCurrent(size_t iterator) const
	{
		return this->GetObjectBase()->GetRenderObjects()[iterator];
	}

	const Matrix4x4& MxObject::GetModel() const
	{
		if (needUpdate)
		{
			auto Translation = MxEngine::Translate(Matrix4x4(1.0f), this->translation);
			auto Rotation = ToMatrix(this->rotation);
			auto Scale = MxEngine::Scale(Matrix4x4(1.0f), this->scale);
			this->Model = Translation * Rotation * Scale;
			needUpdate = false;
		}
		return this->Model;
	}

	bool MxObject::HasShader() const
	{
		return this->Shader != nullptr;
	}

	const MxEngine::Shader& MxObject::GetShader() const
	{
		return *this->Shader;
	}

	bool MxObject::IsDrawable() const
	{
		return this->shouldRender && this->object != nullptr;
	}

	bool MxObject::HasTexture() const
	{
		return this->Texture != nullptr;
	}

	const MxEngine::Texture& MxObject::GetTexture() const
	{
		return *this->Texture;
	}

	size_t MxObject::GetInstanceCount() const
	{
		return this->instanceCount;
	}
}