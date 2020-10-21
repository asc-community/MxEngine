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

    CompoundCollider::~CompoundCollider()
    {
        for (auto& [storage, deleter] : this->children)
        {
            deleter(&storage);
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
        auto& [storage, deleter] = this->children[index];
        deleter(&storage);
        this->children.erase(this->children.begin() + index);
    }

    TransformComponent CompoundCollider::GetShapeTransformByIndex(size_t index)
    {
        return this->compoundShape->GetShapeTransformByIndex(index);
    }

    void CompoundCollider::SetShapeTransformByIndex(size_t index, const TransformComponent& relativeTransform)
    {
        this->compoundShape->SetShapeTransformByIndex(index, relativeTransform);
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
}