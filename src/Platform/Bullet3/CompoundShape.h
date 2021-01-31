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

#include "ShapeBase.h"

class btCompoundShape;

namespace MxEngine
{
    template<typename, typename> class Resource;

    class CompoundShape : public ShapeBase
    {
        btCollisionShape* GetShapeImpl(size_t index) const;
        void AddShapeImpl(btCollisionShape* ptr, size_t userIndex, const TransformComponent& relativeTransform);
        static size_t GetShapeUserHandle(btCollisionShape* ptr);

        template<typename Shape, typename Factory>
        Resource<Shape, Factory> GetShapeByIndexDeduced(size_t index, Resource<Shape, Factory>*) const
        {
            auto shape = this->GetShapeImpl(index);
            auto handle = CompoundShape::GetShapeUserHandle(shape);
            auto& pool = Factory::template Get<Shape>();

            if (pool.Capacity() <= handle) return { };
            auto& managedObject = pool[handle];
            if(managedObject.refCount == 0 || managedObject.uuid == UUIDGenerator::GetNull())
                return { };

            if (managedObject.value.GetNativeHandle() != shape)
                return { };

            return Factory::GetHandle(managedObject);
        }
     
    public:
        using NativeHandle = btCompoundShape*;

        CompoundShape();
        CompoundShape(const CompoundShape&) = delete;
        CompoundShape(CompoundShape&&) noexcept;
        CompoundShape& operator=(const CompoundShape&) = delete;
        CompoundShape& operator=(CompoundShape&&) noexcept;
        ~CompoundShape();

        size_t GetShapeCount() const;
        void RemoveShapeByIndex(size_t index);
        TransformComponent GetShapeTransformByIndex(size_t index) const;
        void SetShapeTransformByIndex(size_t index, const TransformComponent& relativeTransform);
        void ClearShapes();

        template<typename T>
        decltype(auto) GetShapeByIndex(size_t index) const
        {
            return this->GetShapeByIndexDeduced(index, (T*)nullptr);
        }

        template<typename Shape, typename Factory>
        void AddShape(Resource<Shape, Factory> shape, const TransformComponent& relativeTransform)
        {
            this->AddShapeImpl(shape->GetNativeHandle(), shape.GetHandle(), relativeTransform);
        }
    };
}