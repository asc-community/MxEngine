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
		if (!this->GetComponent<MeshSource>().IsValid())
		{
			Logger::Instance().Warning("MxEngine::MxObject", "trying to add buffer to object with no mesh");
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
		this->GetComponent<MeshSource>()->GetMesh()->AddInstancedBuffer(std::move(VBO), std::move(VBL));
	}

	void MxObject::BufferDataByIndex(size_t index, ArrayBufferType buffer, size_t count, size_t offset)
	{
		VertexBuffer& VBO = this->GetComponent<MeshSource>()->GetMesh()->GetBufferByIndex(index);
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
		instance->Model = *this->GetComponent<MxEngine::Transform>();
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
		if (!this->GetComponent<MeshSource>().IsValid())
		{
			Logger::Instance().Warning("MxEngine::MxObject", "GetBufferCount() is called for object with no mesh");
			return 0;
		}
		return this->GetComponent<MeshSource>()->GetMesh()->GetBufferCount();
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
		if (this->GetComponent<MeshSource>().IsValid())
			this->boundingBox = this->GetComponent<MeshSource>()->GetMesh()->GetAABB() * this->GetComponent<MxEngine::Transform>()->GetMatrix();
		else
			this->boundingBox = AABB{ };
		return this->boundingBox;
	}

    MxObject::MxObject()
    {
		this->MeshRenderer = this->AddComponent<MxEngine::MeshRenderer>();
		this->Transform = this->AddComponent<MxEngine::Transform>();
    }

	size_t MxObject::GetInstanceCount() const
	{
		auto instances = this->GetComponent<InstanceFactory>();
		if (!instances.IsValid()) return 0;
		return instances->GetCount();
	}
}