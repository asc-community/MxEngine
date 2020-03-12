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

#include "MxObject.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
	void MxObject::AddInstancedBuffer(ArrayBufferType buffer, size_t count, size_t components, UsageType type)
	{
		if (this->ObjectMesh == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "trying to add buffer to not existing object");
			return;
		}

		if (this->instanceCount == 0) this->instanceCount = count;
		if (this->instanceCount > count)
		{
			Logger::Instance().Error("MxEngine::MxObject", "instance buffer was not added as it contains not enough information");
			return;
		}
		else if (this->instanceCount < count)
		{
			Logger::Instance().Error("MxEngine::MxObject", "instance buffer contains more data than required, additional will be ignored");
		}

		auto VBO = Graphics::Instance()->CreateVertexBuffer(buffer, count * components, type);
		auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
		while (components > 4)
		{
			VBL->PushFloat(4);
			components -= 4;
		}
		VBL->PushFloat(components);
		this->ObjectMesh->AddInstancedBuffer(std::move(VBO), std::move(VBL));
	}

	void MxObject::BufferDataByIndex(size_t index, ArrayBufferType buffer, size_t count, size_t offset)
	{
		VertexBuffer& VBO = this->ObjectMesh->GetBufferByIndex(index);
		VBO.BufferSubData(buffer, count, offset);
	}

	size_t MxObject::GetBufferCount() const
	{
		if (this->ObjectMesh == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "GetBufferCount() is called on null render object");
			return 0;
		}
		return this->ObjectMesh->GetBufferCount();
	}

	MxObject::MxObject(Mesh* mesh)
	{
		this->SetMesh(mesh);
	}

	void MxObject::OnUpdate()
	{
		// this method is overloaded in derived class
	}

	void MxObject::SetMesh(Mesh* mesh)
	{
		this->ObjectMesh = mesh;
	}

	Mesh& MxObject::GetMesh()
	{
		assert(this->ObjectMesh != nullptr);
		return *this->ObjectMesh;
	}

	const Mesh& MxObject::GetMesh() const
	{
		assert(this->ObjectMesh != nullptr);
		return *this->ObjectMesh;
	}

	void MxObject::Hide()
	{
		this->shouldRender = false;
	}

	void MxObject::Show()
	{
		this->shouldRender = true;
	}

	MxObject& MxObject::Translate(float x, float y, float z)
	{
		this->ObjectTransform.Translate(MakeVector3(x, y, z));
		return *this;
	}

	MxObject& MxObject::TranslateForward(float dist)
	{
		this->ObjectTransform.Translate(this->ObjectTransform.GetRotation() * this->forwardVec * dist);
		return *this;
	}

	MxObject& MxObject::TranslateRight(float dist)
	{
		this->ObjectTransform.Translate(this->ObjectTransform.GetRotation() * this->rightVec * dist);
		return *this;
	}

	MxObject& MxObject::TranslateUp(float dist)
	{
		this->ObjectTransform.Translate(this->ObjectTransform.GetRotation() * this->upVec * dist);
		return *this;
	}

	MxObject& MxObject::Rotate(float horz, float vert)
	{
		this->ObjectTransform.Rotate(horz, this->ObjectTransform.GetRotation() * this->upVec);
		this->ObjectTransform.Rotate(vert, this->ObjectTransform.GetRotation() * this->rightVec);
		
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

	MxObject& MxObject::Scale(float x, float y, float z)
	{
		this->ObjectTransform.Scale(MakeVector3(x, y, z));
		return *this;
	}

	MxObject& MxObject::Rotate(float x, float y, float z)
	{
		this->ObjectTransform.Rotate(1.0f, MakeVector3(x, y, z));
		return *this;
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

	size_t MxObject::GetIterator() const
	{
		return 0;
	}

	bool MxObject::IsLast(size_t iterator) const
	{
		return this->GetMesh().GetRenderObjects().size() == iterator;
	}

	size_t MxObject::GetNext(size_t iterator) const
	{
		return iterator + 1;
	}

	const IRenderable& MxObject::GetCurrent(size_t iterator) const
	{
		return this->GetMesh().GetRenderObjects()[iterator];
	}

	const Matrix4x4& MxObject::GetModelMatrix() const
	{
		return this->ObjectTransform.GetMatrix();
	}

	bool MxObject::HasShader() const
	{
		return this->ObjectShader != nullptr;
	}

	const Shader& MxObject::GetShader() const
	{
		return *this->ObjectShader;
	}

	bool MxObject::IsDrawable() const
	{
		return this->shouldRender && this->ObjectMesh != nullptr;
	}

	bool MxObject::HasTexture() const
	{
		return this->ObjectTexture != nullptr;
	}

	const Texture& MxObject::GetTexture() const
	{
		return *this->ObjectTexture;
	}

	size_t MxObject::GetInstanceCount() const
	{
		return this->instanceCount;
	}
}