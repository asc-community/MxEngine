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

#include "DirectionalLight.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
    const Texture* DirectionalLight::GetDepthTexture() const
    {
        return this->texture.get();
    }

    Texture* DirectionalLight::GetDepthTexture()
    {
        return this->texture.get();
    }

    void DirectionalLight::AttachDepthTexture(UniqueRef<Texture> texture)
    {
        this->texture = std::move(texture);
    }

    Matrix4x4 DirectionalLight::GetMatrix() const
    {
        Vector3 Low  = MakeVector3(-this->ProjectionSize);
        Vector3 High = MakeVector3( this->ProjectionSize);
        
        Matrix4x4 OrthoProjection = MakeOrthographicMatrix(Low.x, High.x, Low.y, High.y, Low.z, High.z);
        Matrix4x4 LightView = MakeViewMatrix(
            this->ProjectionCenter + this->Direction,
            this->ProjectionCenter + MakeVector3(0.0f, 0.0f, 0.00001f),
            MakeVector3(0.0f, 1.0f, 0.00001f)
        );
        return OrthoProjection * LightView;
    }
}