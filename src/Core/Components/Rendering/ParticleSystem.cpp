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

#include "ParticleSystem.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void ParticleSystem::Init()
    {
        this->particleBuffer = GraphicFactory::Create<ShaderStorageBuffer>((ParticleGPU*)nullptr, 0, UsageType::STATIC_COPY);
    }

    void ParticleSystem::OnUpdate(float)
    {
        if (this->isDirty)
        {
            MxVector<ParticleGPU> initialState(this->GetMaxParticleCount());
            this->FillParticleData(initialState);
            this->particleBuffer->Load(initialState.data(), initialState.size(), this->particleBuffer->GetUsageType());
            this->isDirty = false;
        }
    }

    ShaderStorageBufferHandle ParticleSystem::GetParticleBuffer() const
    {
        return this->particleBuffer;
    }

    Vector3 GetRandomVector3(ParticleSystem::Shape shape)
    {
        Vector3 result;
        switch (shape)
        {
        case ParticleSystem::Shape::SPHERE:
            result = Random::GetUnitVector3();
            break;
        default:
            result = Vector3(0.0f, 0.0f, 0.0f);
        }
        return result;
    }

    void ParticleSystem::FillParticleData(MxVector<ParticleGPU>& particles) const
    {
        auto& systemTransform = MxObject::GetByComponent(*this).Transform;
        Vector3 systemCenter = systemTransform.GetPosition();

        for (auto& particle : particles)
        {
            particle.Position = this->IsRelative() ? Vector3(0.0f) : systemCenter;
            particle.TimeAlive = Random::GetFloat() * this->GetMaxInitialTimeAlive();
            particle.Velocity = this->GetParticleSpeed() * GetRandomVector3(this->GetShape());
            particle.Size = Random::Range(this->GetMinParticleSize(), this->GetMaxParticleSize());
        }
    }

    void ParticleSystem::SetMaxParticleCount(size_t count)
    {
        this->maxParticleCount = count;
        this->isDirty = true;
    }

    float ParticleSystem::GetParticleLifetime() const
    {
        return this->particleLifetime;
    }

    void ParticleSystem::SetParticleLifetime(float lifetime)
    {
        this->particleLifetime = Max(lifetime, 0.0f);
    }

    float ParticleSystem::GetMinParticleSize() const
    {
        return this->particleMinSize;
    }

    void ParticleSystem::SetMinParticleSize(float size)
    {
        this->particleMinSize = Clamp(size, 0.0f, this->GetMaxParticleSize());
        this->isDirty = true;
    }

    float ParticleSystem::GetMaxParticleSize() const
    {
        return this->particleMaxSize;
    }

    void ParticleSystem::SetMaxParticleSize(float size)
    {
        this->particleMaxSize = Max(size, 0.0f);
        this->isDirty = true;
        this->SetMinParticleSize(this->GetMinParticleSize());
    }

    float ParticleSystem::GetParticleSpeed() const
    {
        return this->particleSpeed;
    }

    void ParticleSystem::SetParticleSpeed(float speed)
    {
        this->particleSpeed = Max(speed, 0.0f);
        this->isDirty = true;
    }

    float ParticleSystem::GetMaxInitialTimeAlive() const
    {
        return this->maxInitialTimeAlive;
    }

    void ParticleSystem::SetMaxInitialTimeAlive(float timeAlive)
    {
        this->maxInitialTimeAlive = Max(timeAlive, 0.0);
        this->isDirty = true;
    }

    bool ParticleSystem::IsRelative() const
    {
        return this->isRelative;
    }

    void ParticleSystem::SetRelative(bool relative)
    {
        this->isRelative = relative;
        this->isDirty = true;
    }

    ParticleSystem::Shape ParticleSystem::GetShape() const
    {
        return this->shape;
    }

    void ParticleSystem::SetShape(Shape shape)
    {
        this->shape = shape;
        this->isDirty = true;
    }

    size_t ParticleSystem::GetMaxParticleCount() const
    {
        return this->maxParticleCount;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::enumeration<ParticleSystem::Shape>("ParticleSystemShape")
        (
            rttr::value("SPHERE", ParticleSystem::Shape::SPHERE)
        );

        rttr::registration::class_<ParticleSystem>("ParticleSystem")
            .constructor<>()
            .property("shape", &ParticleSystem::GetShape, &ParticleSystem::SetShape)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("is relative", &ParticleSystem::IsRelative, &ParticleSystem::SetRelative)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("particle lifetime", &ParticleSystem::GetParticleLifetime, &ParticleSystem::SetParticleLifetime)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max initial time alive", &ParticleSystem::GetMaxInitialTimeAlive, &ParticleSystem::SetMaxInitialTimeAlive)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("min particle size", &ParticleSystem::GetMinParticleSize, &ParticleSystem::SetMinParticleSize)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max particle size", &ParticleSystem::GetMaxParticleSize, &ParticleSystem::SetMaxParticleSize)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("particle speed", &ParticleSystem::GetParticleSpeed, &ParticleSystem::SetParticleSpeed)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max particle count", &ParticleSystem::GetMaxParticleCount, &ParticleSystem::SetMaxParticleCount)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range{ 0.0f, 1000000.0f })
            );
    }
}