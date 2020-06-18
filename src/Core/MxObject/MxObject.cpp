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

#include "MxObject.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Platform/GraphicAPI.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Format/Format.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshRenderer.h"

namespace MxEngine
{
	void MxObject::AddInstancedBuffer(ArrayBufferType buffer, size_t count, size_t components, size_t perComponentFloats, UsageType type)
	{
		if (this->GetMesh() == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "trying to add buffer to not existing object");
			return;
		}

		auto instances = this->GetComponent<InstanceFactory>();

		if (instances.IsValid() && instances->GetCount() > 0)
		{
			if (instances->GetCount() > count)
			{
				Logger::Instance().Warning("MxEngine::MxObject", "instance buffer has less size than object instances count");
			}
			else if (instances->GetCount() < count)
			{
				Logger::Instance().Warning("MxEngine::MxObject", "instance buffer contains more data than instances count");
			}
		}

		auto VBO = MakeUnique<VertexBuffer>(buffer, count * components, type);
		auto VBL = MakeUnique<VertexBufferLayout>();
		while (components > perComponentFloats)
		{
			VBL->PushFloat(perComponentFloats);
			components -= perComponentFloats;
		}
		VBL->PushFloat(components);
		this->GetMesh()->AddInstancedBuffer(std::move(VBO), std::move(VBL));
	}

	void MxObject::BufferDataByIndex(size_t index, ArrayBufferType buffer, size_t count, size_t offset)
	{
		VertexBuffer& VBO = this->GetMesh()->GetBufferByIndex(index);
		if (count <= VBO.GetSize()) // replace data if VBO size is big enough
		{
			MX_ASSERT(offset + count <= VBO.GetSize());
			VBO.BufferSubData(buffer, count, offset);
		}
		else // allocate new buffer (VBO handle remains the same)
		{
			MX_ASSERT(offset == 0);
			VBO.Load(buffer, count, UsageType::DYNAMIC_DRAW);
		}
	}

	InstanceFactory::MxInstance MxObject::Instanciate()
	{
		auto instances = this->GetComponent<InstanceFactory>();
		if (!instances.IsValid())
		{
			constexpr size_t baseSize = 8;
			this->ReserveInstances(baseSize, UsageType::DYNAMIC_DRAW);

			if (!this->HasComponent<InstanceFactory>()) // something went wrong, so we cannot call MakeInstance
			{
				return InstanceFactory::MxInstance{ };
			}
			instances = this->GetComponent<InstanceFactory>();
		}
		auto instance = instances->MakeInstance();
		instance->Model = this->GetTransform();
		instance->SetColor(this->renderColor);
		return instance;
	}

	ComponentView<MxInstanceImpl> MxObject::GetInstances() const
	{
		auto instances = this->GetComponent<InstanceFactory>();
		if (!instances.IsValid())
		{
			Logger::Instance().Warning("MxEngine::MxObject", "object has not been instanced");
			static std::remove_reference<decltype(instances->GetInstancePool())>::type emptyPool;
			return ComponentView { emptyPool };
		}
		return ComponentView{ instances->GetInstancePool() };
	}

	size_t MxObject::GetBufferCount() const
	{
		if (this->GetMesh() == nullptr)
		{
			Logger::Instance().Warning("MxEngine::MxObject", "GetBufferCount() is called on null render object");
			return 0;
		}
		return this->GetMesh()->GetBufferCount();
	}

	void MxObject::ReserveInstances(size_t count, UsageType usage)
	{
		Logger::Instance().Debug("MxEngine::MxObject", MxFormat("making object instanced with reserved count: {0}", count));
		MX_ASSERT(!this->HasComponent<InstanceFactory>());

		auto instances = this->AddComponent<InstanceFactory>();
		instances->InstanceDataIndex = this->GetBufferCount();
		// add model matrix buffer
		this->AddInstancedBuffer(nullptr, count, sizeof(Matrix4x4) / sizeof(float), 4, usage);
		// add normal matrix buffer
		this->AddInstancedBuffer(nullptr, count, sizeof(Matrix3x3) / sizeof(float), 3, usage);
		// add color vector buffer
		this->AddInstancedBuffer(nullptr, count, sizeof(Vector4) / sizeof(float), 4, usage);
	}

    MxObject::MxObjectHandle MxObject::Create()
    {
		auto object = Factory::Create<MxObject>();
		object->Handle = object.GetHandle();
		return object;
    }

	void MxObject::Destroy(MxObject::MxObjectHandle& object)
	{
		Factory::Destroy(object);
	}

    void MxObject::Destroy(MxObject& object)
    {
		MX_ASSERT(object.Handle != InvalidHandle);
		Factory::Get<MxObject>().Deallocate(object.Handle);
    }

    ComponentView<MxObject> MxObject::GetObjects()
    {
		return ComponentView<MxObject>{ Factory::Get<MxObject>() };
    }

	void MxObject::MakeInstanced(size_t instances, UsageType usage) 
	{
		this->ReserveInstances(instances, usage);
		for (size_t i = 0; i < instances; i++)
			this->Instanciate();
	}

	void MxObject::DestroyInstances()
	{
		this->RemoveComponent<InstanceFactory>();
	}

    void MxObject::SetAutoBuffering(bool value)
    {
		this->instanceUpdate = value;
    }

    void MxObject::BufferInstances()
    {
		MX_ASSERT(this->HasComponent<InstanceFactory>());
		MX_ASSERT(this->GetComponent<InstanceFactory>()->InstanceDataIndex != InstanceFactory::Undefined);
		constexpr size_t modelMatrixSize  = sizeof(Matrix4x4) / sizeof(float);
		constexpr size_t normalMatrixSize = sizeof(Matrix3x3) / sizeof(float);
		constexpr size_t colorVectorSize  = sizeof(Vector4) / sizeof(float);

		auto instances = this->GetComponent<InstanceFactory>();

		size_t index  = instances->InstanceDataIndex;
		size_t count  = instances->GetCount();
		auto& models  = instances->GetModelData();
		auto& normals = instances->GetNormalData();
		auto& colors  = instances->GetColorData();
		{
			MAKE_SCOPE_PROFILER("MxObject::BufferInstancesToGPU");
			this->BufferDataByIndex(index, reinterpret_cast<float*>(models.data()), count * modelMatrixSize);
			this->BufferDataByIndex(index + 1, reinterpret_cast<float*>(normals.data()), count * normalMatrixSize);
			this->BufferDataByIndex(index + 2, reinterpret_cast<float*>(colors.data()), count * colorVectorSize);
		}
    }

    const AABB& MxObject::GetAABB() const
    {
		if (this->GetMesh() != nullptr)
			this->boundingBox = this->GetMesh()->GetAABB() * this->GetTransform().GetMatrix();
		else
			this->boundingBox = AABB{ };
		return this->boundingBox;
	}

	Transform& MxObject::GetTransform()
	{
		return *this->GetComponent<Transform>();
	}

    MxObject::MxObject()
    {
		auto meshRenderer = this->AddComponent<MeshRenderer>();
		auto transform = this->AddComponent<Transform>();
    }

	Mesh* MxObject::GetMesh() const
	{
		auto mesh = this->GetComponent<MeshSource>();
		return mesh.IsValid() ? mesh.GetUnchecked()->GetMesh().GetUnchecked() : nullptr;
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
		this->GetTransform().Translate(MakeVector3(x, y, z) * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::TranslateForward(float dist)
	{
		auto& transform = this->GetTransform();
		transform.Translate(transform.GetRotation() * this->forwardVec * dist * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::TranslateRight(float dist)
	{
		auto& transform = this->GetTransform();
		transform.Translate(transform.GetRotation() * this->rightVec * dist * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::TranslateUp(float dist)
	{
		auto& transform = this->GetTransform();
		transform.Translate(transform.GetRotation() * this->upVec * dist * this->TranslateSpeed);
		return *this;
	}

	MxObject& MxObject::Rotate(float horz, float vert)
	{
		auto& transform = this->GetTransform();
		transform.Rotate(this->RotateSpeed * horz, transform.GetRotation() * this->upVec);
		transform.Rotate(this->RotateSpeed * vert, transform.GetRotation() * this->rightVec);
		
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
		this->GetTransform().Scale(MakeVector3(x, y, z) * this->ScaleSpeed);
		return *this;
	}

	MxObject& MxObject::Rotate(float x, float y, float z)
	{
		this->GetTransform().Rotate(this->RotateSpeed, MakeVector3(x, y, z));
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

    const Transform& MxObject::GetTransform() const
    {
		return *this->GetComponent<Transform>();
    }

	size_t MxObject::GetInstanceCount() const
	{
		auto instances = this->GetComponent<InstanceFactory>();
		if (!instances.IsValid()) return 0;
		return instances->GetCount();
	}
}