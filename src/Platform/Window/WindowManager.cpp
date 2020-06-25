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

#include "WindowManager.h"
#include "Core/Application/Application.h"

namespace MxEngine
{   
    #define WND(func, ...) Application::Get()->GetWindow().func(__VA_ARGS__)

    Vector2 WindowManager::GetSize()
    {
        auto width  = (float)WND(GetWidth);
        auto height = (float)WND(GetHeight);
        return MakeVector2(width, height);
    }

    float WindowManager::GetWidth()
    {
        return (float)WND(GetWidth);
    }

    void WindowManager::SetWidth(float width)
    {
        WindowManager::SetSize(MakeVector2(width, WindowManager::GetHeight()));
    }

    void WindowManager::SetHeight(float height)
    {
        WindowManager::SetSize(MakeVector2(WindowManager::GetWidth(), height));
    }

    float WindowManager::GetHeight()
    {
        return (float)WND(GetHeight);
    }

    Vector2 WindowManager::GetPosition()
    {
        return WND(GetWindowPosition);
    }

    const MxString& WindowManager::GetTitle()
    {
        return WND(GetTitle);
    }

    void WindowManager::SetTitle(const MxString& title)
    {
        WND(UseTitle, title);
    }

    void WindowManager::SetPosition(const Vector2& pos)
    {
        WND(UseWindowPosition, (int)pos.x, (int)pos.y);
    }

    void WindowManager::SetSize(const Vector2& size)
    {
        WND(UseWindowSize, (int)size.x, (int)size.y);
    }
}