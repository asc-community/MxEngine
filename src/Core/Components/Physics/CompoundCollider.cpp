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

#include "CompoundCollider.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void CompoundCollider::CreateNewShape()
    {
        this->SetColliderChangedFlag(true);
        this->compoundShape = PhysicsFactory::Create<CompoundShape>();
    }

    void CompoundCollider::Init()
    {
        this->CreateNewShape();
        this->UpdateCollider();
    }

    void CompoundCollider::UpdateCollider()
    {
        auto& self = MxObject::GetByComponent(*this);
        if (this->ShouldUpdateCollider(self))
        {
            this->CreateNewShape();
        }
    }

    CompoundShapeHandle CompoundCollider::GetNativeHandle() const
    {
        return this->compoundShape;
    }

    size_t CompoundCollider::GetShapeCount() const
    {
        return this->compoundShape->GetShapeCount();
    }

    void CompoundCollider::RemoveShapeByIndex(size_t index)
    {
        this->compoundShape->RemoveShapeByIndex(index);
        this->children.erase(this->children.begin() + index);
    }

    TransformComponent CompoundCollider::GetShapeTransformByIndex(size_t index) const
    {
        return this->compoundShape->GetShapeTransformByIndex(index);
    }

    void CompoundCollider::SetShapeTransformByIndex(size_t index, const TransformComponent& relativeTransform)
    {
        this->compoundShape->SetShapeTransformByIndex(index, relativeTransform);
    }

    void CompoundCollider::ClearShapes()
    {
        this->compoundShape->ClearShapes();
        this->children.clear();
    }

    AABB CompoundCollider::GetAABB() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->compoundShape->GetAABB(transform);
    }

    BoundingSphere CompoundCollider::GetBoundingSphere() const
    {
        auto& transform = MxObject::GetByComponent(*this).Transform;
        return this->compoundShape->GetBoundingSphere(transform);
    }

    MxVector<CompoundCollider::CompoundColliderChild> CompoundCollider::GetChildren() const
    {
        MxVector<CompoundColliderChild> result;
        result.resize(this->GetShapeCount());
        for (size_t i = 0; i < result.size(); i++)
        {
            result[i] = { this->GetShapeTransformByIndex(i), this->children[i] };
        }
        return result;
    }

    void CompoundCollider::SetChildren(MxVector<CompoundColliderChild> v)
    {
        this->ClearShapes();
        for (auto& child : v)
        {
            std::visit([this, &child](auto&& shape) { this->AddShape(child.Transform, shape); }, child.Shape);
        }
    }

    MXENGINE_REFLECT_TYPE
    {
        using CompoundColliderChild = CompoundCollider::CompoundColliderChild;
        rttr::registration::class_<CompoundColliderChild>("CompoundColliderChild")
            (
                rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<CompoundColliderChild>)
            )
            .constructor<>()
            (
                rttr::policy::ctor::as_object
            )
            .property("transform", &CompoundColliderChild::Transform)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("shape", &CompoundColliderChild::Shape)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::CUSTOM_VIEW, GUI::EditorExtra<CompoundColliderChild>),
                rttr::metadata(SerializeInfo::CUSTOM_SERIALIZE, SerializeExtra<CompoundColliderChild>),
                rttr::metadata(SerializeInfo::CUSTOM_DESERIALIZE, DeserializeExtra<CompoundColliderChild>)
            );

        rttr::registration::class_<CompoundCollider>("CompoundCollider")
            .constructor<>()
            .method("clear shapes", &CompoundCollider::ClearShapes)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
            .method("add shape", &CompoundCollider::GetNativeHandle)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::CUSTOM_VIEW, GUI::EditorExtra<CompoundCollider>)
            )
            .property("children", &CompoundCollider::GetChildren, &CompoundCollider::SetChildren)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}