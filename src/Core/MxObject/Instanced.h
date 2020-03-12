// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "Core/MxObject/MxObject.h"
#include "Utilities/Logger/Logger.h"

namespace MxEngine
{
    // usage: class MyObject : public Instanced<MxObject>
    // usage: class MyCube : public Instanced<Cube>

    template<typename T, typename = typename std::enable_if<std::is_base_of<MxObject, T>::value>::type>
    class Instanced : public T
    {
        using BufferIndex = size_t;
        using ModelStorage = std::vector<Matrix4x4>;
        using NormalStorage = std::vector<Matrix4x4>;
        using ModelFlags = std::vector<char>;
        constexpr static BufferIndex UndefinedIndex = std::numeric_limits<BufferIndex>::max();

        ModelStorage models;
        NormalStorage normals;
        BufferIndex modelIndex = UndefinedIndex;
        size_t instanceCount = 0;
        ModelFlags needUpdate;
        bool fullUpdate = false;

        template<typename T>
        static const float* AsFloatArray(const T* array) { return reinterpret_cast<const float*>(array); }

        void UpdateAllImpl()
        {
            constexpr size_t matrixSize = sizeof(Matrix4x4) / sizeof(float);
            MxObject* base = static_cast<MxObject*>(this);

            base->BufferDataByIndex(modelIndex, AsFloatArray(models.data()), instanceCount * matrixSize);
            for (size_t i = 0; i < this->instanceCount; i++)
            {
                this->normals[i] = Inverse(Transpose(this->models[i]));
            }
            base->BufferDataByIndex(modelIndex + 1, AsFloatArray(normals.data()), instanceCount * matrixSize);
        }

        void UpdatePartialImpl()
        {
            constexpr size_t matrixSize = sizeof(Matrix4x4) / sizeof(float);
            MxObject* base = static_cast<MxObject*>(this);
            size_t beginIt = 0;
            size_t endIt = 0;

            for (size_t i = 0; i < this->instanceCount; i++)
            {
                // buffer matrix and normal one by one into VBO
                if (needUpdate[i])
                {
                    this->normals[i] = Inverse(Transpose(this->models[i]));
                    endIt++;
                }
                else
                {
                    if (beginIt != endIt)
                    {
                        base->BufferDataByIndex(modelIndex + 0, AsFloatArray(&models[beginIt]), (endIt - beginIt) * matrixSize, beginIt * matrixSize);
                        base->BufferDataByIndex(modelIndex + 1, AsFloatArray(&models[beginIt]), (endIt - beginIt) * matrixSize, beginIt * matrixSize);
                        beginIt = endIt;
                    }
                    beginIt++;
                    endIt++;
                }
            }
            if (beginIt != endIt)
            {
                base->BufferDataByIndex(modelIndex + 0, AsFloatArray(&models[beginIt]), (endIt - beginIt) * matrixSize, beginIt * matrixSize);
                base->BufferDataByIndex(modelIndex + 1, AsFloatArray(&models[beginIt]), (endIt - beginIt) * matrixSize, beginIt * matrixSize);
            }
        }
    public:
        template<typename... Args>
        Instanced(Args&&... args) : T(std::forward<Args>(args)...) { }

        void MakeInstanced(size_t instanceCount, UsageType usage = UsageType::STATIC_DRAW)
        {
            if (modelIndex != UndefinedIndex)
            {
                Logger::Instance().Warning("MxInstanced", "MxInstanced object already initialized");
                return;
            }

            MxObject* base = static_cast<MxObject*>(this);

            this->instanceCount = instanceCount;
            this->modelIndex = base->GetMesh().GetBufferCount();
            this->models.resize(instanceCount);
            this->normals.resize(instanceCount);
            this->needUpdate.resize(instanceCount);
            // add model matrix buffer
            base->AddInstancedBuffer(nullptr, instanceCount, sizeof(Matrix4x4) / sizeof(float), usage);
            // add normal matrix buffer
            base->AddInstancedBuffer(nullptr, instanceCount, sizeof(Matrix4x4) / sizeof(float), usage);
        }

        const Matrix4x4& GetInstanceModel(size_t index) const
        {
            assert(index < this->instanceCount);
            return this->models[index];
        }

        void SetInstanceModel(size_t index, const Matrix4x4& model)
        {
            assert(index < this->instanceCount);
            this->models[index] = model;
            this->needUpdate[index] = true;
        }

        void BufferInstances()
        {
            assert(this->modelIndex != UndefinedIndex);

            if (fullUpdate)
            {
                this->UpdateAllImpl();
            }
            else
            {
                this->UpdatePartialImpl();
            }
            needUpdate.assign(instanceCount, false);
            fullUpdate = false;
        }

        void InvalidateModels()
        {
            fullUpdate = true;
        }

        template<typename G>
        void GenerateInstanceModels(G&& generator)
        {
            static_assert(std::is_same<decltype(generator(0)), Matrix4x4>::value, "generator must return Matrix4x4");
            assert(this->modelIndex != UndefinedIndex);
            this->fullUpdate = true;

            for (size_t i = 0; i < this->instanceCount; i++)
            {
                models[i] = generator(static_cast<int>(i));
            }
        }
    };    
}