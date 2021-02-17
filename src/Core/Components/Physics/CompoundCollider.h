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

#pragma once

#include "Utilities/ECS/Component.h"
#include "Platform/PhysicsAPI.h"
#include "ColliderBase.h"

#include <variant>

namespace MxEngine
{
    class CompoundCollider : public ColliderBase
    {
        MAKE_COMPONENT(CompoundCollider);

    public:
        using VariantType = std::variant<BoxShapeHandle, SphereShapeHandle, CylinderShapeHandle, CapsuleShapeHandle>;
        using VariantArray = MxVector<VariantType>;

        struct CompoundColliderChild
        {
            TransformComponent Transform;
            VariantType Shape;
        };

    private:
        CompoundShapeHandle compoundShape;
        VariantArray children;

        void CreateNewShape();

    public:
        CompoundCollider() = default;
        void Init();
        void UpdateCollider();

        CompoundShapeHandle GetNativeHandle() const;

        size_t GetShapeCount() const;
        void RemoveShapeByIndex(size_t index);
        TransformComponent GetShapeTransformByIndex(size_t index) const;
        const VariantType& GetShapeByIndex(size_t index) const;
        void SetShapeTransformByIndex(size_t index, const TransformComponent& relativeTransform);
        void ClearShapes();

        template<typename T>
        auto GetShapeByIndex(size_t index) const
        {
            using THandle = Resource<T, PhysicsFactory>;
            if (std::holds_alternative<THandle>(this->children[index]))
                return std::get<THandle>(this->children[index]);
            else
                return THandle{ };
        }

        template<typename T, typename... Args>
        void AddShape(const TransformComponent& relativeTransform, Args&&... args)
        {
            auto shape = PhysicsFactory::Create<T>(std::forward<Args>(args)...);
            this->AddShape(relativeTransform, shape);
        }

        template<typename T>
        void AddShape(const TransformComponent& relativeTransform, const Resource<T, PhysicsFactory>& shape)
        {
            this->children.push_back(shape); // save handle in variant array
            this->compoundShape->AddShape(std::move(shape), TransformComponent{ });
            this->SetShapeTransformByIndex(this->GetShapeCount() - 1, relativeTransform);
        }

        AABB GetAABB() const;
        BoundingSphere GetBoundingSphere() const;

        MxVector<CompoundColliderChild> GetChildren() const;
        void SetChildren(MxVector<CompoundColliderChild> v);
    };
}