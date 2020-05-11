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
#include "Utilities/Format/Format.h"

namespace MxEngine
{
	static Instancing<MxObject>::InstanceList DefaultInstancing{ };

	void MxObject::AddInstancedBuffer(ArrayBufferType buffer, size_t count, size_t components, size_t perComponentFloats, UsageType type)
	{
		if (this->ObjectMesh == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "trying to add buffer to not existing object");
			return;
		}

		if (this->instances != nullptr && this->instances->GetCount() > 0)
		{
			if (this->instances->GetCount() > count)
			{
				Logger::Instance().Warning("MxEngine::MxObject", "instance buffer has less size than object instances count");
			}
			else if (this->instances->GetCount() < count)
			{
				Logger::Instance().Warning("MxEngine::MxObject", "instance buffer contains more data than instances count");
			}
		}

		auto VBO = Graphics::Instance()->CreateVertexBuffer(buffer, count * components, type);
		auto VBL = Graphics::Instance()->CreateVertexBufferLayout();
		while (components > perComponentFloats)
		{
			VBL->PushFloat(perComponentFloats);
			components -= perComponentFloats;
		}
		VBL->PushFloat(components);
		this->ObjectMesh->AddInstancedBuffer(std::move(VBO), std::move(VBL));
	}

	void MxObject::BufferDataByIndex(size_t index, ArrayBufferType buffer, size_t count, size_t offset)
	{
		VertexBuffer& VBO = this->ObjectMesh->GetBufferByIndex(index);
		if (count <= VBO.GetSize()) // replace data if VBO size is big enough
		{
			assert(offset + count <= VBO.GetSize());
			VBO.BufferSubData(buffer, count, offset);
		}
		else // allocate new buffer (VBO handle remains the same)
		{
			assert(offset == 0);
			VBO.Load(buffer, count, UsageType::DYNAMIC_DRAW);
		}
	}

	MxInstanceWrapper<MxObject> MxObject::Instanciate()
	{
		if (this->instances == nullptr)
		{
			constexpr size_t baseSize = 8;
			this->ReserveInstances(baseSize, UsageType::DYNAMIC_DRAW);

			if (this->instances == nullptr) // something went wrong, so we cannot call MakeInstance
			{
				MxInstanceWrapper<MxObject> wrapper;
				wrapper.Index = 0;
				wrapper.InstanceList = &DefaultInstancing;
				return wrapper;
			}
		}
		auto instance = this->instances->MakeInstance();
		instance->Model = this->ObjectTransform;
		instance->SetColor(this->renderColor);
		return instance;
	}

	const Instancing<MxObject>::InstanceList& MxObject::GetInstances() const
	{
		if (this->instances == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "object has not been instanced");
			return DefaultInstancing;
		}
		return this->instances->GetInstanceList();
	}

	Instancing<MxObject>::InstanceList& MxObject::GetInstances()
	{
		if (this->instances == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "object has not been instanced");
			return DefaultInstancing;
		}
		return this->instances->GetInstanceList();
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

	void MxObject::ReserveInstances(size_t count, UsageType usage)
	{
		Logger::Instance().Debug("MxEngine::MxObject", Format("making object instanced with reserved count: {0}", count));
		assert(this->instances == nullptr);

		if (this->ObjectMesh != nullptr && this->ObjectMesh->RefCounter > 1)
		{
			Logger::Instance().Error("MxEngine::MxObject", "object which has copies cannot be instanced");
			return;
		}

		this->instances = MakeUnique<Instancing<MxObject>>();
		this->instances->InstanceDataIndex = this->GetBufferCount();
		// add model matrix buffer
		this->AddInstancedBuffer(nullptr, count, sizeof(Matrix4x4) / sizeof(float), 4, usage);
		// add normal matrix buffer
		this->AddInstancedBuffer(nullptr, count, sizeof(Matrix3x3) / sizeof(float), 3, usage);
		// add color vector buffer
		this->AddInstancedBuffer(nullptr, count, sizeof(Vector4) / sizeof(float), 4, usage);
	}

	void MxObject::MakeInstanced(size_t instances, UsageType usage)
	{
		this->ReserveInstances(instances, usage);
		for (size_t i = 0; i < instances; i++)
			this->Instanciate();
	}

	void MxObject::DestroyInstances()
	{
		this->instances->GetInstanceList().resize(0);
	}

    void MxObject::SetAutoBuffering(bool value)
    {
		this->instanceUpdate = value;
    }

    void MxObject::BufferInstances()
    {
		assert(this->instances != nullptr);
		assert(this->instances->InstanceDataIndex != this->instances->Undefined);
		constexpr size_t modelMatrixSize  = sizeof(Matrix4x4) / sizeof(float);
		constexpr size_t normalMatrixSize = sizeof(Matrix3x3) / sizeof(float);
		constexpr size_t colorVectorSize  = sizeof(Vector4) / sizeof(float);

		size_t index  = this->instances->InstanceDataIndex;
		size_t count  = this->instances->GetCount();
		auto& models  = this->instances->GetModelData();
		auto& normals = this->instances->GetNormalData();
		auto& colors  = this->instances->GetColorData();
		{
			MAKE_SCOPE_PROFILER("MxObject::BufferInstancesToGPU");
			this->BufferDataByIndex(index, reinterpret_cast<float*>(models.data()), count * modelMatrixSize);
			this->BufferDataByIndex(index + 1, reinterpret_cast<float*>(normals.data()), count * normalMatrixSize);
			this->BufferDataByIndex(index + 2, reinterpret_cast<float*>(colors.data()), count * colorVectorSize);
		}
    }

    const AABB& MxObject::GetAABB() const
    {
		if (this->ObjectMesh != nullptr)
			this->boundingBox = this->ObjectMesh->GetAABB() * this->ObjectTransform.GetMatrix();
		else
			this->boundingBox = AABB{ };
		return this->boundingBox;
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
		if (this->ObjectMesh != nullptr) 
			this->ObjectMesh->RefCounter--;

		this->ObjectMesh = mesh;
		if(mesh != nullptr)
			this->ObjectMesh->RefCounter++;
	}

	Mesh* MxObject::GetMesh()
	{
		return this->ObjectMesh;
	}

	const Mesh* MxObject::GetMesh() const
	{
		return this->ObjectMesh;
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
		this->ObjectTransform.Translate(MakeVector3(x, y, z) * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::TranslateForward(float dist)
	{
		this->ObjectTransform.Translate(this->ObjectTransform.GetRotation() * this->forwardVec * dist * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::TranslateRight(float dist)
	{
		this->ObjectTransform.Translate(this->ObjectTransform.GetRotation() * this->rightVec * dist * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::TranslateUp(float dist)
	{
		this->ObjectTransform.Translate(this->ObjectTransform.GetRotation() * this->upVec * dist * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::Rotate(float horz, float vert)
	{
		this->ObjectTransform.Rotate(this->RotateSpeed * horz, this->ObjectTransform.GetRotation() * this->upVec);
		this->ObjectTransform.Rotate(this->RotateSpeed * vert, this->ObjectTransform.GetRotation() * this->rightVec);
		
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
		this->ObjectTransform.Scale(MakeVector3(x, y, z) * this->ScaleSpeed);
		return *this;
	}

	MxObject& MxObject::Rotate(float x, float y, float z)
	{
		this->ObjectTransform.Rotate(this->RotateSpeed, MakeVector3(x, y, z));
		return *this;
	}

    void MxObject::SetRenderColor(const Vector4& color)
    {
		this->renderColor = Clamp(color, MakeVector4(0.0f), MakeVector4(1.0f));
    }

	const Vector4& MxObject::GetRenderColor() const
	{
		return this->renderColor;
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
		return this->GetMesh()->GetRenderObjects().size() == iterator;
	}

	size_t MxObject::GetNext(size_t iterator) const
	{
		return iterator + 1;
	}

	const IRenderable& MxObject::GetCurrent(size_t iterator) const
	{
		return this->GetMesh()->GetRenderObjects()[iterator];
	}

    const Transform& MxObject::GetTransform() const
    {
		return this->ObjectTransform;
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
		if (this->instances == nullptr) return 0;
		return this->instances->GetCount();
	}

	void MxObject::OnRenderDraw()
	{
		if (this->instances == nullptr || !this->instanceUpdate) return;
		assert(this->instances->InstanceDataIndex != Instancing<MxObject>::Undefined);
		this->BufferInstances();
	}
}