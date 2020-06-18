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

#include "SpotLight.h"

namespace MxEngine
{
    SpotLight::SpotLight()
    {
        auto texture = GraphicFactory::Create<Texture>();
        texture->LoadDepth(512, 512);
        this->AttachDepthTexture(texture);
    }

    float SpotLight::GetInnerAngle() const
    {
        return this->innerAngle;
    }

    float SpotLight::GetOuterAngle() const
    {
        return this->outerAngle;
    }

    float SpotLight::GetInnerCos() const
    {
        return this->innerCos;
    }

    float SpotLight::GetOuterCos() const
    {
        return this->outerCos;
    }

    SpotLight& SpotLight::UseInnerAngle(float angle)
    {
        this->innerAngle = Clamp(angle, 0.0f, this->outerAngle - 0.0001f);
        this->innerCos = std::cos(Radians(this->innerAngle));
        return *this;
    }

    SpotLight& SpotLight::UseOuterAngle(float angle)
    {
        this->outerAngle = Clamp(angle, 0.0f, 90.0f);
        this->outerCos = std::cos(Radians(this->outerAngle));
        // update inner as it can be larger than outer
        this->UseInnerAngle(this->GetInnerAngle()); 
        return *this;
    }

    GResource<Texture> SpotLight::GetDepthTexture() const
    {
        return this->texture;
    }

    void SpotLight::AttachDepthTexture(const GResource<Texture>& texture)
    {
        this->texture = texture;
    }

    Matrix4x4 SpotLight::GetMatrix(const Vector3& position) const
    {
        auto Projection = MakePerspectiveMatrix(Radians(2.0f * this->outerAngle), 1.0f, 1.1f, 1000.0f);
        auto directionNorm = Normalize(MakeVector3(
            this->Direction.x + 0.0001f,
            this->Direction.y,
            this->Direction.z + 0.0001f
        ));
        auto View = MakeViewMatrix(
            position + directionNorm,
            position,
            MakeVector3(0.0f, 1.0f, 0.0f)
        );
        return Projection * View;
    }
}