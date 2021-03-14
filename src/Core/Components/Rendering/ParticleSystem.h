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

#include "Utilities/ECS/Component.h"
#include "Platform/GraphicAPI.h"

namespace MxEngine
{
    class ParticleSystem
    {
    public:
        struct ParticleGPU
        {
            Vector3 Position;
            float TimeAlive;
            Vector3 Velocity;
            float Size;
            Vector3 _Padding;
            float SpawnDistance;
        };

        enum class Shape
        {
            SPHERE,
            DISK,
        };
    private:
        MAKE_COMPONENT(ParticleSystem);

        ShaderStorageBufferHandle particleBuffer;
        float particleLifetime = 1.0f;
        float particleMinSize = 0.05f;
        float particleMaxSize = 0.05f;
        float particleSpeed = 1.0f;
        float maxInitialTimeAlive = 0.0f;
        float minSpawnDistance = 0.0f;
        float maxSpawnDistance = 0.0f;
        Shape shape = Shape::SPHERE;
        size_t maxParticleCount = 1024;
        bool isDirty = true;
        bool isRelative = false;

        void FillParticleData(MxVector<ParticleGPU>& particles) const;
    public:

        ParticleSystem() = default;

        void Init();

        void OnUpdate(float dt);
        void Invalidate();
        ShaderStorageBufferHandle GetParticleBuffer() const;

        size_t GetMaxParticleCount() const;
        void SetMaxParticleCount(size_t count);

        void SetParticleLifetime(float lifetime);
        float GetParticleLifetime() const;

        float GetMinParticleSize() const;
        void SetMinParticleSize(float size);
        float GetMaxParticleSize() const;
        void SetMaxParticleSize(float size);

        float GetParticleSpeed() const;
        void SetParticleSpeed(float speed);

        float GetMinSpawnDistance() const;
        void SetMinSpawnDistance(float distance);
        float GetMaxSpawnDistance() const;
        void SetMaxSpawnDistance(float distance);

        float GetMaxInitialTimeAlive() const;
        void SetMaxInitialTimeAlive(float timeAlive);

        bool IsRelative() const;
        void SetRelative(bool relative);

        Shape GetShape() const;
        void SetShape(Shape shape);
    };
}