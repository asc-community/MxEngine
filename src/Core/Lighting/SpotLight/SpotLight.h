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

#include "Core/Interfaces/ILightSource.h"

namespace MxEngine
{
    class SpotLight final : public ILightSource
    {
        Vector3 ambientColor  = MakeVector3(1.0f);
        Vector3 diffuseColor  = MakeVector3(1.0f);
        Vector3 specularColor = MakeVector3(1.0f);
        float innerAngle      = 35.0f;
        float innerCos        = std::cos(Radians(innerAngle));
        float outerAngle      = 45.0f;
        float outerCos        = std::cos(Radians(outerAngle));
    public:
        Vector3 Position      = MakeVector3(0.0f);
        Vector3 Direction     = MakeVector3(0.0f, 1.0f, 0.0f);

        float GetInnerAngle() const;
        float GetOuterAngle() const;
        float GetInnerCos() const;
        float GetOuterCos() const;
        SpotLight& UseInnerAngle(float angle);
        SpotLight& UseOuterAngle(float angle);
        SpotLight& UsePosition(const Vector3& position);
        SpotLight& UseDirection(const Vector3& direction);

        // Inherited via ILightSource
        virtual SpotLight& UseAmbientColor(const Vector3& ambient) override;
        virtual SpotLight& UseDiffuseColor(const Vector3& diffuse) override;
        virtual SpotLight& UseSpecularColor(const Vector3& specular) override;
        virtual const Vector3& GetAmbientColor() const override;
        virtual const Vector3& GetDiffuseColor() const override;
        virtual const Vector3& GetSpecularColor() const override;
    };
}