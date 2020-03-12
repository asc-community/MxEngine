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

#include "Utilities/Math/Math.h"

namespace MxEngine
{
    template<typename T>
    class LightSource
    {
    protected:
        inline T& UseAmbientColorImpl(const Vector3& ambient) { return static_cast<T*>(this)->UseAmbientColor(ambient); }
        inline T& UseDiffuseColorImpl(const Vector3& diffuse) { return static_cast<T*>(this)->UseDiffuseColor(diffuse); }
        inline T& UseSpecularColorImpl(const Vector3& specular) { return static_cast<T*>(this)->UseSpecularColor(specular); }
        inline const Vector3& GetAmbientColorImpl() const { return static_cast<T*>(this)->GetAmbientColor(); }
        inline const Vector3& GetDiffuseColorImpl() const { return static_cast<T*>(this)->GetDiffuseColor(); }
        inline const Vector3& GetSpecularColorImpl() const { return static_cast<T*>(this)->GetSpecularColor(); }

        virtual ~LightSource() = default;
    };
}