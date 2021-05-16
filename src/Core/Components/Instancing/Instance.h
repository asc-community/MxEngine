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

#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
    class Instance
    {
        MAKE_COMPONENT(Instance);

        Vector3 color{ 1.0f };
        MxObject::Handle parent;
    public:
        Instance(const MxObject::Handle& parent) : parent(parent) { }

        MxObject::Handle GetParent() { return this->parent; }

        void SetColor(const Vector3& color)
        {
            this->color = Clamp(color, MakeVector3(0.0f), MakeVector3(1.0f));
        }

        const Vector3& GetColor() const
        {
            return this->color;
        }
    };

    bool IsInstanced(const MxObject& object);
    bool IsInstanced(MxObject::Handle object);
    bool IsInstance(const MxObject& object);
    bool IsInstance(MxObject::Handle object);
    MxObject::Handle Instanciate(MxObject& object);
    MxObject::Handle Instanciate(MxObject::Handle object);
    MxObject::Handle GetInstanceParent(const MxObject& object);
    MxObject::Handle GetInstanceParent(MxObject::Handle object);
}