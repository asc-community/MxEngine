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

#include "Platform/GraphicAPI.h"
#include "Utilities/ECS/Component.h"

namespace MxEngine
{
    class Skybox
    {
    public:
        constexpr static float DefaultIntensity = 0.5f;
    private:
        MAKE_COMPONENT(Skybox);

        Vector3 rotation = MakeVector3(0.0f);
        float intensity = Skybox::DefaultIntensity;
    public:
        Skybox() = default;

        // CubeMapHandle CubeMap;
        // CubeMapHandle Irradiance;

        void SetIntensity(float intensity) { this->intensity = Max(intensity, 0.0f); }
        float GetIntensity() const { return this->intensity; }

        Quaternion GetRotationQuaternion() const { return MakeQuaternion(MakeRotationMatrix(RadiansVec(this->rotation))); }
        void SetRotation(const Quaternion& q) { this->SetRotation(DegreesVec(MakeEulerAngles(q))); }

        void RotateX(float angle) { this->Rotate(Vector3(angle, 0.0f, 0.0f)); }
        void RotateY(float angle) { this->Rotate(Vector3(0.0f, angle, 0.0f)); }
        void RotateZ(float angle) { this->Rotate(Vector3(0.0f, 0.0f, angle)); }
        const Vector3& GetRotation() const { return this->rotation; }

        void SetRotation(const Vector3& angles)
        {
            this->rotation = MakeVector3(0.0f);
            this->Rotate(angles);
        }

        void Rotate(const Vector3& angles)
        {
            this->rotation += angles;
            this->rotation.x = std::fmod(this->rotation.x, 360.0f);
            this->rotation.y = std::fmod(this->rotation.y, 360.0f);
            this->rotation.z = std::fmod(this->rotation.z, 360.0f);
        }
    };
}

MXENGINE_FORCE_REFLECTION(Skybox);