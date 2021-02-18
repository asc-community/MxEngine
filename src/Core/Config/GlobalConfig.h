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

#include "Config.h"

namespace MxEngine
{
	class GlobalConfig
	{
	public:
        static BuildType GetBuildType();
        static const Vector2& GetWindowPosition();
        static const Vector2& GetWindowSize();
        static const MxString& GetWindowTitle();
        static CursorMode GetCursorMode();
        static bool HasDoubleBuffering();
        static RenderProfile GetGraphicAPIProfile();
        static size_t GetGraphicAPIMajorVersion();
        static size_t GetGraphicAPIMinorVersion();
        static size_t GetAnisothropicFiltering();;
        static size_t GetDirectionalLightTextureSize();
        static size_t GetPointLightTextureSize();
        static size_t GetSpotLightTextureSize();
        static size_t GetEngineTextureSize();
        static const MxVector<MxString>& GetIgnoredFolders();
        static const MxString& GetShaderSourceDirectory();
        static EditorStyle GetEditorStyle();
        static bool HasGraphicAPIDebug();
        static bool HasAutoRecompileFiles();
        static bool HasCachePrimitiveModels();
        static KeyCode GetApplicationCloseKey();
        static KeyCode GetEditorOpenKey();
        static KeyCode GetRecompileFilesKey();
	};
}