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
#include "Core/Macro/Macro.h"

#define IM_VEC2_CLASS_EXTRA\
    ImVec2(const MxEngine::Vector2& f) { x = f.x; y = f.y; }\
    operator MxEngine::Vector2() const { return MxEngine::MakeVector2(x,y); }

#define IM_VEC4_CLASS_EXTRA\
    ImVec4(const MxEngine::Vector4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }\
    operator MxEngine::Vector4() const { return MxEngine::MakeVector4(x,y,z,w); }

#include "Vendors/imgui/imgui.h"

#if defined(MXENGINE_USE_OPENGL)
    #define IMGUI_IMPL_OPENGL_LOADER_GLEW
    #include "Vendors/imgui/imgui_impl_glfw.h"
    #include "Vendors/imgui/imgui_impl_opengl3.h"
#endif