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

#include "Utilities/Math/Math.h"
#include "Utilities/STL/MxString.h"
#include "Utilities/Json/Json.h"
#include "Core/Events/KeyEvent.h"

namespace MxEngine
{
    enum class RenderProfile : uint8_t
    {
        ANY,
        COMPAT,
        CORE,
    };

    enum class CursorMode : uint8_t
    {
        NORMAL,
        HIDDEN,
        DISABLED,
    };

    enum class BuildType : uint8_t
    {
        UNKNOWN,
        DEBUG,
        RELEASE,
        SHIPPING,
    };

    enum class EditorStyle : uint8_t
    {
        CLASSIC,
        LIGHT,
        DARK,
        MXENGINE,
    };

    const char* EnumToString(CursorMode mode);
    const char* EnumToString(RenderProfile profile);
    const char* EnumToString(BuildType mode);
    const char* EnumToString(EditorStyle style);

    struct Config
    {
        // Window settings
        Vector2 WindowPosition{ 300, 150 };
        Vector2 WindowSize{ 1600, 900 };
        MxString WindowTitle = "MxEngine Application";
        CursorMode Cursor = CursorMode::DISABLED;
        bool DoubleBuffering = false;

        // Renderer settings
        RenderProfile GraphicAPIProfile = RenderProfile::CORE;
        size_t GraphicAPIMajorVersion = 4;
        size_t GraphicAPIMinorVersion = 5;
        size_t AnisothropicFiltering = 16;
        size_t DirLightTextureSize = 2048;
        size_t PointLightTextureSize = 512;
        size_t SpotLightTextureSize = 512;
        size_t EngineTextureSize = 512;

        // Filesystem settings
        MxString ProjectRootDirectory = "./Resources";
        MxString ShaderSourceDirectory = "../../src/Platform/OpenGL/Shaders";

        // Debug settings
        bool GraphicAPIDebug = true;
        bool AutoRecompileFiles = false;
        EditorStyle Style = EditorStyle::MXENGINE;
        KeyCode ApplicationCloseKey = KeyCode::ESCAPE;
        KeyCode EditorOpenKey = KeyCode::GRAVE_ACCENT;
        KeyCode RecompileFilesKey = KeyCode::F5;
    };

    void Deserialize(Config& config, const JsonFile& json);
    void Serialize(JsonFile& json, const Config& config);

    void to_json(JsonFile& j, CursorMode mode);
    void from_json(const JsonFile& j, CursorMode& mode);
    void to_json(JsonFile& j, RenderProfile profile);
    void from_json(const JsonFile& j, RenderProfile& profile);
    void to_json(JsonFile& j, KeyCode key);
    void from_json(const JsonFile& j, KeyCode& key);
    void to_json(JsonFile& j, EditorStyle style);
    void from_json(const JsonFile& j, EditorStyle& style);
}