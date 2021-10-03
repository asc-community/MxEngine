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

#include "Core/Events/EventBase.h"
#include "VulkanAbstractionLayer/Window.h"

#include <bitset>

namespace MxEngine
{
    using KeyCode = VulkanAbstractionLayer::KeyCode;
    using CursorMode = VulkanAbstractionLayer::CursorMode;

    const char* EnumToString(KeyCode key);

    class KeyEvent : public EventBase
    {
        MAKE_EVENT(KeyEvent);

        using KeyVectorPointer = const std::bitset<350>*;

        // pointer to bitset. Is is okay as event lives less than bitset
        KeyVectorPointer keyHeld;
        KeyVectorPointer keyPressed;
        KeyVectorPointer keyReleased;
    public:
        inline KeyEvent(KeyVectorPointer held, KeyVectorPointer pressed, KeyVectorPointer released) noexcept
            : keyHeld(held), keyPressed(pressed), keyReleased(released) { }

        inline bool IsHeld(KeyCode key)     const { return (key != KeyCode::UNKNOWN) && (*keyHeld)[size_t(key)]; }
        inline bool IsPressed(KeyCode key)  const { return (key != KeyCode::UNKNOWN) && (*keyPressed)[size_t(key)]; }
        inline bool IsReleased(KeyCode key) const { return (key != KeyCode::UNKNOWN) && (*keyReleased)[size_t(key)]; }
    };
}