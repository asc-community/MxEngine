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

#include "Config.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    template<typename T>
    void FromJson(T& dist, const JsonFile& json, const char* name)
    {
        dist = json.value(name, dist);
    }

    const char* EnumToString(BuildType mode)
    {
        switch (mode)
        {
        case BuildType::UNKNOWN:
            return "UNKNOWN";
        case BuildType::DEBUG:
            return "DEBUG";
        case BuildType::RELEASE:
            return "RELEASE";
        case BuildType::SHIPPING:
            return "SHIPPING";
        default:
            return "UNKNOWN";
        }
    }

    const char* EnumToString(EditorStyle style)
    {
        switch (style)
        {
        case MxEngine::EditorStyle::CLASSIC:
            return "CLASSIC";
        case MxEngine::EditorStyle::LIGHT:
            return "LIGHT";
        case MxEngine::EditorStyle::DARK:
            return "DARK";
        case MxEngine::EditorStyle::MXENGINE:
            return "MXENGINE";
        default:
            return "MXENGINE";
        }
    }

    void Deserialize(Config& config, const JsonFile& json)
    {
        FromJson(config.WindowPosition,         json["window"],      "position"                );
        FromJson(config.WindowSize,             json["window"],      "size"                    );
        FromJson(config.WindowTitle,            json["window"],      "title"                   );
        FromJson(config.Cursor,                 json["window"],      "cursor-mode"             );
        FromJson(config.GraphicAPIMajorVersion, json["renderer"],    "major-version"           );
        FromJson(config.GraphicAPIMinorVersion, json["renderer"],    "minor-version"           );
        FromJson(config.AnisothropicFiltering,  json["renderer"],    "anisothropic-filtering"  );
        FromJson(config.DirLightTextureSize,    json["renderer"],    "dir-light-texture-size"  );
        FromJson(config.PointLightTextureSize,  json["renderer"],    "point-light-texture-size");
        FromJson(config.SpotLightTextureSize,   json["renderer"],    "spot-light-texture-size" );
        FromJson(config.EngineTextureSize,      json["renderer"],    "engine-texture-size"     );
        FromJson(config.IgnoredFolders,         json["filesystem" ], "ignored-folders"         );
        FromJson(config.CachePrimitiveModels,   json["filesystem" ], "cache-primitives"        );
        FromJson(config.ShaderSourceDirectory,  json["debug-build"], "shader-source-directory" );
        FromJson(config.ApplicationCloseKey,    json["debug-build"], "app-close-key"           );
        FromJson(config.Style,                  json["debug-build"], "editor-style"            );
        FromJson(config.EditorOpenKey,          json["debug-build"], "editor-key"              );
        FromJson(config.GraphicAPIDebug,        json["debug-build"], "debug-graphics"          );
        FromJson(config.RecompileFilesKey,      json["debug-build"], "recompile-files-key"     );
        FromJson(config.AutoRecompileFiles,     json["debug-build"], "auto-recompile-files"    );
    }

    void Serialize(JsonFile& json, const Config& config)
    {
        json["window"     ]["position"                ] = config.WindowPosition;
        json["window"     ]["size"                    ] = config.WindowSize;
        json["window"     ]["title"                   ] = config.WindowTitle;
        json["window"     ]["cursor-mode"             ] = config.Cursor;
        json["renderer"   ]["major-version"           ] = config.GraphicAPIMajorVersion;
        json["renderer"   ]["minor-version"           ] = config.GraphicAPIMinorVersion;
        json["renderer"   ]["anisothropic-filtering"  ] = config.AnisothropicFiltering;
        json["renderer"   ]["dir-light-texture-size"  ] = config.DirLightTextureSize;
        json["renderer"   ]["point-light-texture-size"] = config.PointLightTextureSize;
        json["renderer"   ]["spot-light-texture-size" ] = config.SpotLightTextureSize;
        json["renderer"   ]["engine-texture-size"     ] = config.EngineTextureSize;
        json["filesystem" ]["ignored-folders"         ] = config.IgnoredFolders;
        json["filesystem" ]["cache-primitives"        ] = config.CachePrimitiveModels;
        json["debug-build"]["shader-source-directory" ] = config.ShaderSourceDirectory;
        json["debug-build"]["app-close-key"           ] = config.ApplicationCloseKey;
        json["debug-build"]["editor-style"            ] = config.Style;
        json["debug-build"]["editor-key"              ] = config.EditorOpenKey;
        json["debug-build"]["debug-graphics"          ] = config.GraphicAPIDebug;
        json["debug-build"]["recompile-files-key"     ] = config.RecompileFilesKey;
        json["debug-build"]["auto-recompile-files"    ] = config.AutoRecompileFiles;
    }

    void to_json(JsonFile& j, EditorStyle style)
    {
        j = EnumToString(style);
    }

    void from_json(const JsonFile& j, EditorStyle& style)
    {
        auto val = j.get<MxString>();
        if (val == "CLASSIC")
            style = EditorStyle::CLASSIC;
        else if (val == "DARK")
            style = EditorStyle::DARK;
        else if (val == "LIGHT")
            style = EditorStyle::LIGHT;
        else
            style = EditorStyle::MXENGINE;
    }
}

namespace VulkanAbstractionLayer
{
    void to_json(MxEngine::JsonFile& j, VulkanAbstractionLayer::CursorMode mode)
    {
        j = MxEngine::EnumToString(mode);
    }

    void from_json(const MxEngine::JsonFile& j, VulkanAbstractionLayer::CursorMode& mode)
    {
        auto str = j.get<MxEngine::MxString>();
        auto t = rttr::type::get<CursorMode>().get_enumeration();
        mode = t.name_to_value(str.c_str()).convert<CursorMode>();
    }

    void to_json(MxEngine::JsonFile& j, VulkanAbstractionLayer::KeyCode key)
    {
        j = MxEngine::EnumToString(key);
    }

    void from_json(const MxEngine::JsonFile& j, VulkanAbstractionLayer::KeyCode& key)
    {
        auto str = j.get<MxEngine::MxString>();
        auto t = rttr::type::get<KeyCode>().get_enumeration();
        key = t.name_to_value(str.c_str()).convert<KeyCode>();
    }
}