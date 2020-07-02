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

#include "Core/Components/Instance.h"

namespace MxEngine
{
    class InstanceView
    {
    public:
        using Pool = VectorPool<MxObject::Handle>;

        class InstanceIterator
        {
        public:
            using PoolIterator = typename Pool::PoolIterator;

        private:
            PoolIterator it;

        public:
            InstanceIterator(PoolIterator it) : it(it) {}

            InstanceIterator operator++(int)
            {
                InstanceIterator copy = *this;
                it++;
                return copy;
            }

            InstanceIterator operator++()
            {
                ++it;
                return *this;
            }

            InstanceIterator operator--(int)
            {
                InstanceIterator copy = *this;
                it--;
                return copy;
            }

            InstanceIterator operator--()
            {
                --it;
                return *this;
            }

            MxObject::Handle& operator->()
            {
                return *it;
            }

            const MxObject::Handle& operator->() const
            {
                return *it;
            }

            MxObject::Handle& operator*()
            {
                return *it;
            }

            const MxObject::Handle& operator*() const
            {
                return *it;
            }

            bool operator==(const InstanceIterator& other) const
            {
                return it == other.it;
            }

            bool operator!=(const InstanceIterator& other) const
            {
                return it != other.it;
            }
        };
    private:
        Pool& ref;

    public:
        explicit InstanceView(Pool& ref) : ref(ref) {}

        InstanceIterator begin()
        {
            return InstanceIterator{ ref.begin() };
        }

        const InstanceIterator begin() const
        {
            return InstanceIterator{ ref.begin() };
        }

        InstanceIterator end()
        {
            return InstanceIterator{ ref.end() };
        }

        const InstanceIterator end() const
        {
            return InstanceIterator{ ref.end() };
        }
    };

	class InstanceFactory
	{
		MAKE_COMPONENT(InstanceFactory);
	public:
		using InstancePool = VectorPool<MxObject::Handle>;
		using ModelData = MxVector<Matrix4x4>;
		using NormalData = MxVector<Matrix3x3>;
		using ColorData = MxVector<Vector3>;
		using BufferIndex = uint16_t;
	private:
		InstancePool pool;
		ModelData models;
		NormalData normals;
		ColorData colors;
		BufferIndex bufferIndex = std::numeric_limits<BufferIndex>::max();

		template<typename T>
		BufferIndex AddInstancedBuffer(Mesh& mesh, const MxVector<T>& data)
		{
			auto VBO = GraphicFactory::Create<VertexBuffer>();
			auto VBL = GraphicFactory::Create<VertexBufferLayout>();
			VBL->Push<T>();
			VBO->Load((float*)data.data(), data.size() * sizeof(T) / sizeof(float), UsageType::DYNAMIC_DRAW);
			return (BufferIndex)mesh.AddInstancedBuffer(std::move(VBO), std::move(VBL));
		}

		template<typename T>
		void BufferDataByIndex(const Mesh& mesh, size_t index, const MxVector<T>& buffer)
		{
			auto VBO = mesh.GetBufferByIndex(index);
			VBO->BufferDataWithResize((float*)buffer.data(), buffer.size() * sizeof(T) / sizeof(float));
		}

		void RemoveInstancedBuffer(Mesh& mesh, size_t index);
		void RemoveDanglingHandles();
		void Destroy();
	public:
		bool IsStatic = false;

		const InstancePool& GetInstancePool() const { return this->pool; }
		InstancePool& GetInstancePool() { return this->pool; };
		size_t GetCount() const { return this->GetInstancePool().Allocated(); }
        InstanceView GetInstances() { return InstanceView{ this->GetInstancePool() }; }

		void Init();
		void OnUpdate();

		void SubmitInstances();
		MxObject::Handle MakeInstance();
		void DestroyInstances();

		ModelData& GetModelData();
		NormalData& GetNormalData();
		ColorData& GetColorData();

		InstanceFactory() = default;
		InstanceFactory(const InstanceFactory&) = delete;
		InstanceFactory(InstanceFactory&&) noexcept = delete;
		InstanceFactory& operator=(const InstanceFactory&) = delete;
		InstanceFactory& operator=(InstanceFactory&&) noexcept = delete;
		~InstanceFactory();
	};
}