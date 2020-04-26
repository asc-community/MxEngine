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

#include "Utilities/Math/Math.h"
#include "Core/Event/Events/KeyEvent.h"

namespace MxEngine
{
	enum class Profile
	{
		ANY,
		COMPAT,
		CORE,
	};

	enum class CursorMode
	{
		NORMAL,
		HIDDEN,
		DISABLED,
	};

	struct WindowHandler;

	struct Window
	{
		virtual int GetHeight() const = 0;
		virtual int GetWidth() const = 0;
		virtual bool IsOpen() const = 0;
		virtual void PullEvents() const = 0;
		virtual void OnUpdate() = 0;

		virtual Vector2 GetCursorPos() const = 0;
		virtual Vector2 GetWindowPos() const = 0;
		virtual CursorMode GetCursorMode() const = 0;
		virtual bool IsKeyHeld(KeyCode key) const = 0;
		virtual bool IsKeyPressed(KeyCode key) const = 0;
		virtual bool IsKeyReleased(KeyCode key) const = 0;
		virtual WindowHandler* GetNativeHandler() const = 0;
		virtual bool IsCreated() const = 0;

		virtual Window& Create() = 0;
		virtual Window& Close() = 0;
		virtual Window& SwitchContext() = 0;
		virtual Window& UseDebugging(bool value = true) = 0;
		virtual Window& UseDoubleBuffering(bool value = true) = 0;
		virtual Window& UseCursorMode(CursorMode cursor) = 0;
		virtual Window& UseCursorPos(const Vector2& pos) = 0;
		virtual Window& UseTitle(const std::string& title) = 0;
		virtual Window& UsePosition(float xpos, float ypos) = 0;
		virtual Window& UseSize(int width, int height) = 0;
		virtual Window& UseEventDispatcher(AppEventDispatcher* dispatcher) = 0;
		virtual Window& UseProfile(int majorVersion, int minorVersion, Profile profile) = 0;
	};
}