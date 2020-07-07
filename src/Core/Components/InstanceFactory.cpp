#include "InstanceFactory.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
    void InstanceFactory::InitMesh()
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (meshSource.IsValid())
        {
            auto& mesh = *meshSource->Mesh;
            auto modelBufferIndex = this->AddInstancedBuffer(mesh, this->GetModelData());
            auto normalBufferIndex = this->AddInstancedBuffer(mesh, this->GetNormalData());
            auto colorBufferIndex = this->AddInstancedBuffer(mesh, this->GetColorData());
            this->bufferIndex = modelBufferIndex; // others will be `bufferIndex + 1`, `bufferIndex + 2`
        }
    }

    void InstanceFactory::RemoveInstancedBuffer(Mesh& mesh, size_t index)
    {
        MX_ASSERT(mesh.GetBufferCount() == index + 1);
        mesh.PopInstancedBuffer();
    }

    void InstanceFactory::RemoveDanglingHandles()
    {
        for (auto& object : this->pool)
        {
            if (!object.IsValid())
                this->pool.Deallocate(this->pool.IndexOf(object));
        }
    }

    void InstanceFactory::Destroy()
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();

        this->DestroyInstances();

        if (meshSource.IsValid())
        {
            auto& mesh = *meshSource->Mesh;
            this->RemoveInstancedBuffer(mesh, (size_t)this->bufferIndex + 2);
            this->RemoveInstancedBuffer(mesh, (size_t)this->bufferIndex + 1);
            this->RemoveInstancedBuffer(mesh, (size_t)this->bufferIndex + 0);
        }
    }

    void InstanceFactory::Init()
    {
        this->InitMesh();
    }

    void InstanceFactory::OnUpdate(float timeDelta)
    {
        this->RemoveDanglingHandles();
        if (!this->IsStatic) this->SubmitInstances();
    }

    MxObject::Handle InstanceFactory::MakeInstance()
    {
        auto object = MxObject::GetHandleByComponent(*this);

        auto instance = MxObject::Create();
        this->pool.Allocate(instance);
        instance->Transform = object->Transform;
        auto component = instance->AddComponent<Instance>(object);
        return std::move(instance);
    }

    void InstanceFactory::DestroyInstances()
    {
        for (auto& object : this->pool)
        {
            MxObject::Destroy(object);
        }
        this->pool.Clear();
    }

    InstanceFactory::ModelData& InstanceFactory::GetModelData()
    {
        MAKE_SCOPE_PROFILER("Instancing::BufferModelData");

        this->models.resize(this->GetCount());
        auto model = this->models.begin();
        auto instance = this->GetInstancePool().begin();

        for (; model != this->models.end(); model++, instance++)
        {
            instance->GetUnchecked()->Transform.GetMatrix(*model);
        }
        if (this->models.empty()) this->models.emplace_back(0.0f);

        return this->models;
    }

    InstanceFactory::NormalData& InstanceFactory::GetNormalData()
    {
        MAKE_SCOPE_PROFILER("Instancing::BufferNormalData");

        this->normals.resize(this->GetCount());
        auto normal = this->normals.begin();
        auto model = this->models.begin();
        auto instance = this->GetInstancePool().begin();
        for (; normal != this->normals.end(); model++, normal++, instance++)
        {
            instance->GetUnchecked()->Transform.GetNormalMatrix(*model, *normal);
        }
        if (this->normals.empty()) this->normals.emplace_back(0.0f);

        return this->normals;
    }

    InstanceFactory::ColorData& InstanceFactory::GetColorData()
    {
        MAKE_SCOPE_PROFILER("Instancing::BufferColorData");

        this->colors.resize(this->GetCount());
        auto instance = this->GetInstancePool().begin();
        for (auto it = this->colors.begin(); it != this->colors.end(); it++, instance++)
        {
            *it = instance->GetUnchecked()->GetComponent<Instance>()->GetColor();
        }
        if (this->colors.empty()) this->colors.emplace_back(0.0f);

        return this->colors;
    }

    InstanceFactory::~InstanceFactory()
    {
        this->Destroy();
    }

    void InstanceFactory::SubmitInstances()
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (meshSource.IsValid())
        {
            auto& mesh = *meshSource->Mesh;

            if ((uint16_t)mesh.GetBufferCount() < this->bufferIndex + 2)
            {
                this->InitMesh(); // MeshSource was updated, re-init mesh
            }
            else
            {
                this->BufferDataByIndex(mesh, (size_t)this->bufferIndex + 0, this->GetModelData());
                this->BufferDataByIndex(mesh, (size_t)this->bufferIndex + 1, this->GetNormalData());
                this->BufferDataByIndex(mesh, (size_t)this->bufferIndex + 2, this->GetColorData());
            }
        }
    }
}