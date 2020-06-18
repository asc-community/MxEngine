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
#include "LightBase.h"

namespace MxEngine
{
    class PointLight : public LightBase
    {
        MAKE_COMPONENT(PointLight);

        GResource<CubeMap> cubemap;
        Vector3 factors   = MakeVector3(1.0f, 0.009f, 0.032f);
    public:
        PointLight();

        float FarDistance = 1000.0f;

        constexpr static size_t Constant  = 0;
        constexpr static size_t Linear    = 1;
        constexpr static size_t Quadratic = 2;

        PointLight& UseFactors(const Vector3& factors);
        [[nodiscard]] const Vector3& GetFactors() const;

        [[nodiscard]] GResource<CubeMap> GetDepthCubeMap() const;
        void AttachDepthCubeMap(const GResource<CubeMap>& cubemap);
        [[nodiscard]] Matrix4x4 GetMatrix(size_t index, const Vector3& position) const;
    };
}