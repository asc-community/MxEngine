#include "InstanceFactory.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Components/Rendering/MeshSource.h"

namespace MxEngine
{
    void InstanceFactory::Init()
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (!meshSource.IsValid())
        {
            Logger::Instance().Warning("MxEngine::InstanceFactory", "InstanceFactory component is added for object with no mesh: " + object.Name);
        }
        else
        {
            auto& mesh = *meshSource->GetMesh();
            auto modelBufferIndex  = this->AddInstancedBuffer(mesh, this->models);
            auto normalBufferIndex = this->AddInstancedBuffer(mesh, this->normals);
            auto colorBufferIndex  = this->AddInstancedBuffer(mesh, this->colors);
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
        return this->models;
    }

    InstanceFactory::NormalData& InstanceFactory::GetNormalData()
    {
        MAKE_SCOPE_PROFILER("Instancing::BufferNormalData");

        this->normals.resize(this->GetInstancePool().Allocated());
        auto normal = this->normals.begin();
        auto model = this->models.begin();
        auto instance = this->GetInstancePool().begin();
        for (; model != this->models.end(); model++, normal++, instance++)
        {
            instance->value.Transform.GetNormalMatrix(*model, *normal);
        }
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
        return this->colors;
    }

    void InstanceFactory::SubmitInstances()
    {
        if (this->bufferIndex == std::numeric_limits<BufferIndex>::max()) return;

        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (meshSource.IsValid())
        {
            auto& mesh = *meshSource->GetMesh();
            this->BufferDataByIndex(mesh, this->bufferIndex + 0, this->GetModelData());
            this->BufferDataByIndex(mesh, this->bufferIndex + 1, this->GetNormalData());
            this->BufferDataByIndex(mesh, this->bufferIndex + 2, this->GetColorData());
        }
    }
}