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

    void ParticleSystem::Invalidate()
    {
        this->isDirty = true;
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
        {
            result = Random::GetUnitVector3();
            break;
        }
        case ParticleSystem::Shape::HEMISPHERE:
        {
            result = Random::GetUnitVector3();
            result.y = std::abs(result.y);
            break;
        }
        case ParticleSystem::Shape::DISK:
        {
            Vector2 v = Random::GetUnitVector2();
            result = MakeVector3(v.x, 0.0f, v.y);
            break;
        }
        case ParticleSystem::Shape::RAY:
        {
            result = MakeVector3(0.0f, 1.0f, 0.0f);
            break;
        }
        case ParticleSystem::Shape::LINE:
        {
            result = MakeVector3(0.0f, Random::GetBool() ? 1.0f : -1.0f, 0.0f);
            break;
        }
        case ParticleSystem::Shape::CROSS:
        {
            int rng = Random::Range(0, 4);
            switch (rng)
            {
            case 0: result = MakeVector3 (1.0f, 0.0f,  0.0f); break;
            case 1: result = MakeVector3(-1.0f, 0.0f,  0.0f); break;
            case 2: result = MakeVector3( 0.0f, 0.0f,  1.0f); break;
            case 3: result = MakeVector3( 0.0f, 0.0f, -1.0f); break;
            }
            break;
        }
        case ParticleSystem::Shape::AXIS:
        {
            int rng = Random::Range(0, 6);
            switch (rng)
            {
            case 0: result = MakeVector3( 1.0f,  0.0f,  0.0f); break;
            case 1: result = MakeVector3(-1.0f,  0.0f,  0.0f); break;
            case 2: result = MakeVector3( 0.0f,  1.0f,  0.0f); break;
            case 3: result = MakeVector3( 0.0f, -1.0f,  0.0f); break;
            case 4: result = MakeVector3( 0.0f,  0.0f,  1.0f); break;
            case 5: result = MakeVector3( 0.0f,  0.0f, -1.0f); break;
            }
            break;
        }
        default:
            result = Vector3(0.0f, 0.0f, 0.0f);
        }
        return result;
    }

    void ParticleSystem::FillParticleData(MxVector<ParticleGPU>& particles) const
    {
        auto& systemTransform = MxObject::GetByComponent(*this).Transform;
        Matrix4x4 particleTransform = this->IsRelative() ? Matrix4x4(1.0f) : systemTransform.GetMatrix();

        for (auto& particle : particles)
        {
            auto shapeBasedRandom = Matrix3x3(particleTransform) * GetRandomVector3(this->GetShape());
            float angularSpeed = this->GetParticleAngularSpeed() == 0.0f ? 0.0001f : this->GetParticleAngularSpeed();
            float linearSpeed = this->GetParticleSpeed() == 0.0f ? 0.0001f : this->GetParticleSpeed();

            particle.AngularParams = Matrix3x3(particleTransform) * Vector3(0.0f, 1.0f, 0.0f) * angularSpeed;
            particle.SpawnDistance = Random::Range(this->GetMinSpawnDistance(), this->GetMaxSpawnDistance());
            particle.Position = particle.SpawnDistance * shapeBasedRandom + Vector3(particleTransform[3]);
            particle.Velocity = linearSpeed * shapeBasedRandom;
            particle.TimeAlive = Random::GetFloat() * this->GetMaxInitialTimeAlive();
            particle.Size = Random::Range(this->GetMinParticleSize(), this->GetMaxParticleSize());
        }
    }

    void ParticleSystem::SetMaxParticleCount(size_t count)
    {
        this->maxParticleCount = count;
        this->Invalidate();
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
        this->Invalidate();
    }

    float ParticleSystem::GetMaxParticleSize() const
    {
        return this->particleMaxSize;
    }

    void ParticleSystem::SetMaxParticleSize(float size)
    {
        this->particleMaxSize = Max(size, 0.0f);
        this->SetMinParticleSize(this->GetMinParticleSize());
        this->Invalidate();
    }

    float ParticleSystem::GetParticleSpeed() const
    {
        return this->particleSpeed;
    }

    void ParticleSystem::SetParticleSpeed(float speed)
    {
        this->particleSpeed = speed;
        this->Invalidate();
    }

    float ParticleSystem::GetParticleAngularSpeed() const
    {
        return this->particleAngularSpeed;
    }

    void ParticleSystem::SetParticleAngularSpeed(float speed)
    {
        this->particleAngularSpeed = speed;
        this->Invalidate();
    }

    float ParticleSystem::GetMinSpawnDistance() const
    {
        return this->minSpawnDistance;
    }

    void ParticleSystem::SetMinSpawnDistance(float distance)
    {
        this->minSpawnDistance = Clamp(distance, 0.0f, this->GetMaxSpawnDistance());
        this->Invalidate();
    }

    float ParticleSystem::GetMaxSpawnDistance() const
    {
        return this->maxSpawnDistance;
    }

    void ParticleSystem::SetMaxSpawnDistance(float distance)
    {
        this->maxSpawnDistance = Max(distance, 0.0f);
        this->SetMinSpawnDistance(this->GetMinSpawnDistance());
        this->Invalidate();
    }

    float ParticleSystem::GetMaxInitialTimeAlive() const
    {
        return this->maxInitialTimeAlive;
    }

    void ParticleSystem::SetMaxInitialTimeAlive(float timeAlive)
    {
        this->maxInitialTimeAlive = Max(timeAlive, 0.0);
        this->Invalidate();
    }

    bool ParticleSystem::IsRelative() const
    {
        return this->isRelative;
    }

    void ParticleSystem::SetRelative(bool relative)
    {
        this->isRelative = relative;
        this->Invalidate();
    }

    float ParticleSystem::GetFading() const
    {
        return this->fading;
    }

    void ParticleSystem::SetFading(float fading)
    {
        this->fading = Max(fading, 0.0f);
    }

    ParticleSystem::Shape ParticleSystem::GetShape() const
    {
        return this->shape;
    }

    void ParticleSystem::SetShape(Shape shape)
    {
        this->shape = shape;
        this->Invalidate();
    }

    size_t ParticleSystem::GetMaxParticleCount() const
    {
        return this->maxParticleCount;
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::enumeration<ParticleSystem::Shape>("ParticleSystemShape")
        (
            rttr::value("SPHERE", ParticleSystem::Shape::SPHERE),
            rttr::value("HEMISPHERE", ParticleSystem::Shape::HEMISPHERE),
            rttr::value("DISK", ParticleSystem::Shape::DISK),
            rttr::value("RAY", ParticleSystem::Shape::RAY),
            rttr::value("LINE", ParticleSystem::Shape::LINE),
            rttr::value("CROSS", ParticleSystem::Shape::CROSS),
            rttr::value("AXIS", ParticleSystem::Shape::AXIS)
        );

        rttr::registration::class_<ParticleSystem>("ParticleSystem")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY | MetaInfo::CLONE_INSTANCE)
            )
            .constructor<>()
            .method("invalidate", &ParticleSystem::Invalidate)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
            )
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
            .property("fading", &ParticleSystem::GetFading, &ParticleSystem::SetFading)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000.0f }),
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
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("particle angular speed", &ParticleSystem::GetParticleAngularSpeed, &ParticleSystem::SetParticleAngularSpeed)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("min spawn distance", &ParticleSystem::GetMinSpawnDistance, &ParticleSystem::SetMinSpawnDistance)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1000000.0f }),
                rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
            )
            .property("max spawn distance", &ParticleSystem::GetMaxSpawnDistance, &ParticleSystem::SetMaxSpawnDistance)
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