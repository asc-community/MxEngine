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

        static inline GResource<Texture> MakeTexture(uint8_t r, uint8_t g, uint8_t b)
        {
            auto texture = GraphicFactory::Create<Texture>();
            uint8_t buffer[] = { r, g, b };
            texture->Load(buffer, 1, 1);
            return texture;
        }

        static inline GResource<Texture> MakeTexture(float r, float g, float b)
        {
            return MakeTexture(MakeVector3(r, g, b));
        }

        static inline GResource<Texture> MakeTexture(const Vector3& color)
        {
            uint8_t r = static_cast<uint8_t>(Clamp(color.r, 0.0f, 1.0f) * 255.0f);
            uint8_t g = static_cast<uint8_t>(Clamp(color.g, 0.0f, 1.0f) * 255.0f);
            uint8_t b = static_cast<uint8_t>(Clamp(color.b, 0.0f, 1.0f) * 255.0f);
            return MakeTexture(r, g, b);
        }

        static GResource<Texture> MakeTexture(Palette color)
        {
            return MakeTexture(ColorPaletteToVector3(color));
        }

        static inline GResource<CubeMap> MakeCubeMap(uint8_t r, uint8_t g, uint8_t b)
        {
            auto cubemap = GraphicFactory::Create<CubeMap>();
            uint8_t buffer[] = { r, g, b };
            std::array<uint8_t*, 6> sides = { buffer, buffer, buffer, buffer, buffer, buffer };
            cubemap->Load(sides, 1, 1);
            return cubemap;
        }

        static inline GResource<CubeMap> MakeCubeMap(float r, float g, float b)
        {
            return MakeCubeMap(MakeVector3(r, g, b));
        }

        static inline GResource<CubeMap> MakeCubeMap(const Vector3& color)
        {
            uint8_t r = static_cast<uint8_t>(Clamp(color.r, 0.0f, 1.0f) * 255.0f);
            uint8_t g = static_cast<uint8_t>(Clamp(color.g, 0.0f, 1.0f) * 255.0f);
            uint8_t b = static_cast<uint8_t>(Clamp(color.b, 0.0f, 1.0f) * 255.0f);
            return MakeCubeMap(r, g, b);
        }

        static GResource<CubeMap> MakeCubeMap(Palette color)
        {
            return MakeCubeMap(ColorPaletteToVector3(color));
        }

        static Vector3 ColorPaletteToVector3(Palette color)
        {
            switch (color)
            {
            case Colors::RED:
                return MakeVector3(1.0f, 0.0f, 0.0f);
            case Colors::GREEN:
                return MakeVector3(0.0f, 1.0f, 0.0f);
            case Colors::BLUE:
                return MakeVector3(0.0f, 0.0f, 1.0f);
            case Colors::YELLOW:
                return MakeVector3(1.0f, 1.0f, 0.0f);
            case Colors::AQUA:
                return MakeVector3(0.0f, 1.0f, 1.0f);
            case Colors::PURPLE:
                return MakeVector3(1.0f, 0.0f, 1.0f);
            case Colors::BLACK:
                return MakeVector3(0.0f, 0.0f, 0.0f);
            case Colors::WHITE:
                return MakeVector3(1.0f, 1.0f, 1.0f);
            case Colors::ORANGE:
                return MakeVector3(1.0f, 0.5f, 0.0f);
            case Colors::LIME:
                return MakeVector3(0.5f, 1.0f, 0.0f);
            case Colors::MAGENTA:
                return MakeVector3(1.0f, 0.0f, 0.5f);
            case Colors::VIOLET:
                return MakeVector3(0.5f, 0.0f, 1.0f);
            case Colors::SKYBLUE:
                return MakeVector3(0.0f, 0.5f, 1.0f);
            case Colors::SPRING:
                return MakeVector3(0.0f, 1.0f, 0.5f);
            case Colors::FLAT_NORMAL:
                return MakeVector3(0.5f, 0.5f, 1.0f);
            case Colors::PINK:
                return MakeVector3(1.0f, 0.5f, 0.5f);
            case Colors::SALAD:
                return MakeVector3(0.5f, 1.0f, 0.5f);
            case Colors::GREY:
                return MakeVector3(0.5f, 0.5f, 0.5f);
            default:
                return MakeVector3(0.0f);
            }
        }
    };
}