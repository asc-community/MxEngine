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

#include "GlobalConfig.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    #define CFG(name) Application::Get()->GetConfig().name

    BuildType GlobalConfig::GetBuildType()
    {
        #if defined(MXENGINE_SHIPPING)
        return BuildType::SHIPPING;
        #elif defined(MXENGINE_RELEASE)
        return BuildType::RELEASE;
        #elif defined(MXENGINE_DEBUG)
        return BuildType::DEBUG;
        #else
        return BuildType::UNKNOWN;
        #endif
    }

    const Vector2& GlobalConfig::GetWindowPosition()
    {
        return CFG(WindowPosition);
    }

    const Vector2& GlobalConfig::GetWindowSize()
    {
        return CFG(WindowSize);
    }

    const MxString& GlobalConfig::GetWindowTitle()
    {
        return CFG(WindowTitle);
    }

    CursorMode GlobalConfig::GetCursorMode()
    {
        return CFG(Cursor);
    }

    bool GlobalConfig::HasDoubleBuffering()
    {
        return CFG(DoubleBuffering);
    }

    RenderProfile GlobalConfig::GetGraphicAPIProfile()
    {
        return CFG(GraphicAPIProfile);
    }

    size_t GlobalConfig::GetGraphicAPIMajorVersion()
    {
        return CFG(GraphicAPIMajorVersion);
    }

    size_t GlobalConfig::GetGraphicAPIMinorVersion()
    {
        return CFG(GraphicAPIMinorVersion);
    }

    size_t GlobalConfig::GetAnisothropicFiltering()
    {
        return CFG(AnisothropicFiltering);
    }

    size_t GlobalConfig::GetDebugLineWidth()
    {
        return CFG(DebugLineWidth);
    }

    size_t GlobalConfig::GetDirectionalLightTextureSize()
    {
        return CFG(DirLightTextureSize);
    }

    size_t GlobalConfig::GetPointLightTextureSize()
    {
        return CFG(PointLightTextureSize);
    }

    size_t GlobalConfig::GetSpotLightTextureSize()
    {
        return CFG(SpotLightTextureSize);
    }

    size_t GlobalConfig::GetBloomTextureSize()
    {
        return CFG(BloomTextureSize);
    }

    const MxString& GlobalConfig::GetProjectRootDirectory()
    {
        return CFG(ProjectRootDirectory);
    }

    EditorStyle GlobalConfig::GetEditorStyle()
    {
        return CFG(Style);
    }

    bool GlobalConfig::HasGraphicAPIDebug()
    {
        return CFG(GraphicAPIDebug);
    }

    KeyCode GlobalConfig::GetApplicationCloseKey()
    {
        return CFG(ApplicationCloseKey);
    }

    KeyCode GlobalConfig::GetEditorOpenKey()
    {
        return CFG(EditorOpenKey);
    }
}