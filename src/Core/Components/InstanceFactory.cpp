#include "InstanceFactory.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Components/Rendering/MeshSource.h"

namespace MxEngine
{
    void InstanceFactory::RemoveInstancedBuffer(Mesh& mesh, size_t index)
    {
        MX_ASSERT(mesh.GetBufferCount() == index + 1);
        mesh.PopInstancedBuffer();
    }

    void InstanceFactory::Destroy()
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (meshSource.IsValid())
        {
            auto& mesh = *meshSource->Mesh;
            this->RemoveInstancedBuffer(mesh, this->bufferIndex + 2);
            this->RemoveInstancedBuffer(mesh, this->bufferIndex + 1);
            this->RemoveInstancedBuffer(mesh, this->bufferIndex + 0);
        }
    }

    void InstanceFactory::Init()
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (meshSource.IsValid())
        {
            auto& mesh = *meshSource->Mesh;
            auto modelBufferIndex  = this->AddInstancedBuffer(mesh, this->GetModelData());
            auto normalBufferIndex = this->AddInstancedBuffer(mesh, this->GetNormalData());
            auto colorBufferIndex  = this->AddInstancedBuffer(mesh, this->GetColorData());
            this->bufferIndex = modelBufferIndex; // others will be `bufferIndex + 1`, `bufferIndex + 2`
        }
    }

    InstanceFactory::MxInstance InstanceFactory::MakeInstance()
    {
        auto& object = MxObject::GetByComponent(*this);

        auto uuid = UUIDGenerator::Get();
        size_t index = this->GetInstancePool().Allocate(uuid);
        auto instance = MxInstance(uuid, index, &this->factory);
        instance->Transform = *object.Transform;
        instance.MakeStatic();
        return std::move(instance);
    }

    InstanceFactory::ModelData& InstanceFactory::GetModelData()
    {
        MAKE_SCOPE_PROFILER("Instancing::BufferModelData");

        this->models.resize(this->GetInstancePool().Allocated());
        auto model = this->models.begin();
        auto instance = this->GetInstancePool().begin();
        for (; model != this->models.end(); model++, instance++)
        {
            instance->value.Transform.GetMatrix(*model);
        }
        if (this->models.empty()) this->models.emplace_back(0.0f);

        return this->models;
    }

    InstanceFactory::NormalData& InstanceFactory::GetNormalData()
    {
        MAKE_SCOPE_PROFILER("Instancing::BufferNormalData");

        this->normals.resize(this->GetInstancePool().Allocated());
        auto normal = this->normals.begin();
        auto model = this->models.begin();
        auto instance = this->GetInstancePool().begin();
        for (; normal != this->normals.end(); model++, normal++, instance++)
        {
            instance->value.Transform.GetNormalMatrix(*model, *normal);
        }
        if (this->normals.empty()) this->normals.emplace_back(0.0f);

        return this->normals;
    }

    InstanceFactory::ColorData& InstanceFactory::GetColorData()
    {
        MAKE_SCOPE_PROFILER("Instancing::BufferColorData");

        this->colors.resize(this->GetInstancePool().Allocated());
        auto instance = this->GetInstancePool().begin();
        for (auto it = this->colors.begin(); it != this->colors.end(); it++, instance++)
        {
            *it = instance->value.GetColor();
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
        if (this->bufferIndex == std::numeric_limits<BufferIndex>::max()) return;

        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (meshSource.IsValid())
        {
            auto& mesh = *meshSource->Mesh;

            if (mesh.GetBufferCount() < this->bufferIndex + 2)
            {
                this->Init(); // MeshSource was updated, re-init mesh
            }
            else
            {
                this->BufferDataByIndex(mesh, this->bufferIndex + 0, this->GetModelData());
                this->BufferDataByIndex(mesh, this->bufferIndex + 1, this->GetNormalData());
                this->BufferDataByIndex(mesh, this->bufferIndex + 2, this->GetColorData());
            }
        }
    }
}