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

#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Core/Application/Application.h"
#include "Core/Macro/Macro.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
    class Colors
    {
    public:
        enum Palette
        {
            RED,
            GREEN,
            BLUE,
            YELLOW,
            AQUA,
            PURPLE,
            BLACK,
            WHITE,
            ORANGE,
            LIME,
            MAGENTA,
            VIOLET,
            SKYBLUE,
            SPRING,
            FLAT_NORMAL,
            PINK,
            SALAD,
            GREY,
        };

        static inline Texture* MakeTexture(uint8_t r, uint8_t g, uint8_t b)
        {
            auto& manager = Application::Get()->GetGlobalScene().GetResourceManager<Texture>();
            auto name = Format(FMT_STRING("MxColor{0}_{1}_{2}"), r, g, b);
            
            if (!manager.Exists(name))
            {
                auto texture = Graphics::Instance()->CreateTexture();
                uint8_t buffer[3];
                buffer[0] = r;
                buffer[1] = g;
                buffer[2] = b;
                texture->Load(buffer, 1, 1);
                manager.Add(name, std::move(texture));
            }                
            return manager.Get(name);
        }

        static inline Texture* MakeTexture(float r, float g, float b)
        {
            return MakeTexture(MakeVector3(r, g, b));
        }

        static inline Texture* MakeTexture(const Vector3& color)
        {
            uint8_t r = static_cast<uint8_t>(Clamp(color.r, 0.0f, 1.0f) * 255.0f);
            uint8_t g = static_cast<uint8_t>(Clamp(color.g, 0.0f, 1.0f) * 255.0f);
            uint8_t b = static_cast<uint8_t>(Clamp(color.b, 0.0f, 1.0f) * 255.0f);
            return MakeTexture(r, g, b);
        }

        static Texture* MakeTexture(Palette color)
        {
            switch (color)
            {
            case Colors::RED:
                return MakeTexture((uint8_t)255, 0,   0  );
            case Colors::GREEN:
                return MakeTexture((uint8_t)0,   255, 0  );
            case Colors::BLUE:
                return MakeTexture((uint8_t)0,   0,   255);
            case Colors::YELLOW:
                return MakeTexture((uint8_t)255, 255, 0  );
            case Colors::AQUA:
                return MakeTexture((uint8_t)0,   255, 255);
            case Colors::PURPLE:
                return MakeTexture((uint8_t)255, 0,   255);
            case Colors::BLACK:
                return MakeTexture((uint8_t)0,   0,   0  );
            case Colors::WHITE:
                return MakeTexture((uint8_t)255, 255, 255);
            case Colors::ORANGE:
                return MakeTexture((uint8_t)255, 127, 0  );
            case Colors::LIME:
                return MakeTexture((uint8_t)127, 255, 0  );
            case Colors::MAGENTA:
                return MakeTexture((uint8_t)255, 0,   127);
            case Colors::VIOLET:
                return MakeTexture((uint8_t)127, 0,   255);
            case Colors::SKYBLUE:
                return MakeTexture((uint8_t)0,   127, 255);
            case Colors::SPRING:
                return MakeTexture((uint8_t)0,   255, 127);
            case Colors::FLAT_NORMAL:
                return MakeTexture((uint8_t)127, 127, 255);
            case Colors::PINK:
                return MakeTexture((uint8_t)255, 127, 127);
            case Colors::SALAD:
                return MakeTexture((uint8_t)127, 255, 127);
            case Colors::GREY:
                return MakeTexture((uint8_t)127, 127, 127);
            default:
                return nullptr;
            }
        }
    };
}