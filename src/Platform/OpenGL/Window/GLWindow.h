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

#include <string>
#include <bitset>

#include "Utilities/Time/Time.h"
#include "Core/Interfaces/IEvent.h"
#include "Utilities/Math/Math.h"
#include "Core/Event/Events/KeyEvent.h"
#include "Core/Interfaces/GraphicAPI/Window.h"

struct GLFWwindow; // from glfw header file

namespace MxEngine
{
	class GLWindow final : public Window
	{
	private:
		std::string title;
		GLFWwindow* window = nullptr;
		int width = 0, height = 0;
		AppEventDispatcher* dispatcher = nullptr;
		mutable std::bitset<350> keyHeld;
		mutable std::bitset<350> keyPressed;
		mutable std::bitset<350> keyReleased;
		CursorMode cursorMode = CursorMode::NORMAL;
		Vector2 windowPosition{ 0.0f, 0.0f };
		Vector2 cursorPosition{ 0.0f, 0.0f };
		Vector2 windowSize{ 0.0f, 0.0f };

		void Destroy();
	public:
		GLWindow() = default;
		GLWindow(int width, int height, const std::string& title);
		GLWindow(const GLWindow&) = delete;
		~GLWindow();

		// Inherited via Window
		virtual int GetHeight() const override;
		virtual int GetWidth() const override;
		virtual bool IsOpen() const override;
		virtual void PullEvents() const override;
		virtual void OnUpdate() override;
		virtual Vector2 GetCursorPos() const override;
		virtual Vector2 GetWindowPos() const override;
		virtual CursorMode GetCursorMode() const override;
		virtual bool IsKeyHeld(KeyCode key) const override;
		virtual bool IsKeyPressed(KeyCode key) const override;
		virtual bool IsKeyReleased(KeyCode key) const override;
		virtual WindowHandler* GetNativeHandler() const override;
		virtual AppEventDispatcher& GetEventDispatcher();
		virtual bool IsCreated() const override;
		virtual GLWindow& Create() override;
		virtual GLWindow& Close() override;
		virtual GLWindow& SwitchContext() override;
		virtual GLWindow& UseDebugging(bool value = true) override;
		virtual GLWindow& UseDoubleBuffering(bool value = true) override;
		virtual GLWindow& UseCursorMode(CursorMode cursor) override;
		virtual GLWindow& UseCursorPos(const Vector2& pos) override;
		virtual GLWindow& UseTitle(const std::string& title) override;
		virtual GLWindow& UsePosition(float xpos, float ypos) override;
		virtual GLWindow& UseSize(int width, int height) override;
		virtual GLWindow& UseEventDispatcher(AppEventDispatcher* dispatcher) override;
		virtual GLWindow& UseProfile(int majorVersion, int minorVersion, Profile profile) override;
	};
}