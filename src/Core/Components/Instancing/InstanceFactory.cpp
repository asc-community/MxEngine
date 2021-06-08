#include "InstanceFactory.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Runtime/Reflection.h"
#include "Core/Resources/BufferAllocator.h"

namespace MxEngine
{
    void InstanceFactory::RemoveDanglingHandles()
    {
        for (auto& object : this->pool)
        {
            if (!object.IsValid() || !object->HasComponent<Instance>())
                this->pool.Deallocate(this->pool.IndexOf(object));
        }
    }

    void InstanceFactory::OnUpdate(float timeDelta)
    {
        this->RemoveDanglingHandles();
        if (!this->IsStatic) 
            this->SendInstancesToGPU();
    }

    void InstanceFactory::SubmitInstances()
    {
        this->RemoveDanglingHandles();
        this->SendInstancesToGPU();
    }

    void InstanceFactory::DestroyInstances()
    {
        this->FreeInstancePool();
    }

    // see SceneSerializer.cpp
    void CloneInstanceInternal(const MxObject::Handle& origin, MxObject::Handle& target);

    MxObject::Handle InstanceFactory::Instanciate()
    {
        auto instance = MxObject::Create();
        auto object = MxObject::GetHandleByComponent(*this);

        this->pool.Allocate(instance);
        this->ReserveInstanceAllocation(this->pool.Capacity());

        auto instanceComponent = instance->AddComponent<Instance>(object);
        CloneInstanceInternal(object, instance);

        return instance;
    }

    void InstanceFactory::UpdateInstanceCache()
    {
        MAKE_SCOPE_PROFILER("Instancing::UpdateInstanceCache");

        this->instances.resize(this->GetInstanceCount());
        auto instanceData = this->instances.begin();
        auto instance = this->GetInstancePool().begin();

        for (; instanceData != this->instances.end(); instanceData++, instance++)
        {
            instance->GetUnchecked()->LocalTransform.GetMatrix(instanceData->Model);
            instance->GetUnchecked()->LocalTransform.GetNormalMatrix(instanceData->Model, instanceData->Normal);
            instanceData->Color = instance->GetUnchecked()->GetComponent<Instance>()->GetColor();
        }
    }

    void InstanceFactory::FreeInstancePool()
    {
        for (auto& object : this->pool)
        {
            MxObject::Destroy(object);
        }
        this->pool.Clear();
    }

    void InstanceFactory::FreeInstanceAllocation()
    {
        if (this->instanceAllocation.Size != 0)
        {
            BufferAllocator::DeallocateInInstanceVBO({ this->instanceAllocation.Offset * InstanceDataSize, this->instanceAllocation.Size * InstanceDataSize });
        }
    }

    InstanceFactory::~InstanceFactory()
    {
        this->FreeInstancePool();
        this->FreeInstanceAllocation();
    }

    void InstanceFactory::SendInstancesToGPU()
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (meshSource.IsValid() && meshSource->Mesh.IsValid())
        {
            this->UpdateInstanceCache();
            BufferAllocator::GetInstanceVBO()->BufferSubData(
                (float*)this->instances.data(),
                this->instances.size() * InstanceDataSize,
                this->instanceAllocation.Offset * InstanceDataSize
            );
        }
    }

    void InstanceFactory::ReserveInstanceAllocation(size_t count)
    {
        if (count <= this->instanceAllocation.Size) return;

        this->FreeInstanceAllocation();

        auto allocation = BufferAllocator::AllocateInInstanceVBO(count * InstanceDataSize);
        this->instanceAllocation.Offset = allocation.Offset / InstanceDataSize;
        this->instanceAllocation.Size = allocation.Size / InstanceDataSize;
    }

    bool IsInstanced(const MxObject& object)
    {
        return object.HasComponent<InstanceFactory>();
    }

    bool IsInstanced(MxObject::Handle object)
    {
        return object.IsValid() && IsInstanced(*object);
    }

    bool IsInstance(const MxObject& object)
    {
        return object.HasComponent<Instance>();
    }

    bool IsInstance(MxObject::Handle object)
    {
        return object.IsValid() && IsInstance(*object);
    }

    MxObject::Handle Instanciate(MxObject& object)
    {
        auto instanceFactory = object.GetOrAddComponent<InstanceFactory>();
        return instanceFactory->Instanciate();
    }

    MxObject::Handle Instanciate(MxObject::Handle object)
    {
        return object.IsValid() ? Instanciate(*object) : MxObject::Handle{ };
    }

    MxObject::Handle GetInstanceParent(const MxObject& object)
    {
        auto instance = object.GetComponent<Instance>();
        return instance.IsValid() ? instance->GetParent() : MxObject::Handle{ };
    }

    MxObject::Handle GetInstanceParent(MxObject::Handle object)
    {
        return object.IsValid() ? GetInstanceParent(*object) : MxObject::Handle{ };
    }

    Transform GetGlobalTransform(const MxObject& object)
    {
        return IsInstance(object) ? LocalToWorld(GetGlobalTransform(GetInstanceParent(object)), object.LocalTransform) : object.LocalTransform;
    }

    Transform GetGlobalTransform(MxObject::Handle object)
    {
        return object.IsValid() ? GetGlobalTransform(*object) : Transform{ };
    }

    MXENGINE_REFLECT_TYPE
    {
        using GetPoolFunc = VectorPool<MxObject::Handle>& (InstanceFactory::*)();

        rttr::registration::class_<Instance>("Instance")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY)
            )
            .property("color", &Instance::GetColor, &Instance::SetColor)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
            );

        rttr::registration::class_<InstanceFactory>("InstanceFactory")
            .constructor<>()
            .method("instanciate", &InstanceFactory::Instanciate)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .method("destroy insances", &InstanceFactory::DestroyInstances)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property("is static", &InstanceFactory::IsStatic)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property_readonly("instance count", &InstanceFactory::GetInstanceCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .property_readonly("instances", (GetPoolFunc)&InstanceFactory::GetInstancePool)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::CUSTOM_VIEW, GUI::EditorExtra<InstanceFactory>),
                rttr::metadata(SerializeInfo::CUSTOM_SERIALIZE, SerializeExtra<InstanceFactory>),
                rttr::metadata(SerializeInfo::CUSTOM_DESERIALIZE, DeserializeExtra<InstanceFactory>)
            );
    }
}