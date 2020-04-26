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

#include "GLWindow.h"

#include "Utilities/Logger/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Event/Events/MouseEvent.h"
#include "Core/Event/Events/WindowResizeEvent.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"
#include "Utilities/Format/Format.h"

#include <array>

namespace MxEngine
{
	void GLWindow::Destroy()
	{
		if (this->window != nullptr)
		{
			this->Close();
			glfwDestroyWindow(this->window);
			Logger::Instance().Debug("MxEngine::Window", "window destroyed");
		}
	}

	GLWindow::GLWindow(int width, int height, const std::string& title)
		: title(title), width(width), height(height)
	{		
		Logger::Instance().Debug("MxEngine::Window", "window object created");
	}

	int GLWindow::GetWidth() const
	{
		if (this->window == nullptr)
			return this->width;
		
		int width, height;
		glfwGetWindowSize(this->window, &width, &height);
		return width;
	}

	WindowHandler* GLWindow::GetNativeHandler() const
	{
		return reinterpret_cast<WindowHandler*>(this->window);
	}

	AppEventDispatcher& GLWindow::GetEventDispatcher()
	{
		return *this->dispatcher;
	}

	bool GLWindow::IsCreated() const
	{
		return this->window != nullptr;
	}

	bool GLWindow::IsOpen() const
	{
		if (this->window == nullptr)
		{
			Logger::Instance().Warning("MxEngine::Window", "window was not created while calling Window::IsOpen");
			return false;
		}
		bool isOpen = !glfwWindowShouldClose(this->window);

		return isOpen;
	}

	void GLWindow::PullEvents() const
	{		
		MAKE_SCOPE_PROFILER("Window::PullEvents");
		this->keyPressed.reset();
		this->keyReleased.reset();

		glfwPollEvents();

		if (this->dispatcher != nullptr)
		{
			auto keyEvent = MakeUnique<KeyEvent>(&this->keyHeld, &this->keyPressed, &this->keyReleased);
			this->dispatcher->AddEvent(std::move(keyEvent));
			auto mouseMoveEvent = MakeUnique<MouseMoveEvent>(this->cursorPosition.x, this->cursorPosition.y);
			this->dispatcher->AddEvent(std::move(mouseMoveEvent));
		}
	}

	void GLWindow::OnUpdate()
	{
		auto& module = Graphics::Instance()->GetGraphicModule();
		module.OnWindowUpdate(this->GetNativeHandler());

		if (this->dispatcher != nullptr)
		{
			Vector2 currentSize(this->GetWidth(), this->GetHeight());
			if (currentSize != this->windowSize)
			{
				this->dispatcher->AddEvent(MakeUnique<WindowResizeEvent>(this->windowSize, currentSize));
				this->windowSize = currentSize;
			}
		}
	}

	GLWindow& GLWindow::Close()
	{
		if (this->window != nullptr && IsOpen())
		{
			auto& module = Graphics::Instance()->GetGraphicModule();
			module.OnWindowDestroy(this->GetNativeHandler());

			glfwSetWindowShouldClose(this->window, true);
			Logger::Instance().Debug("MxEngine::Window", "window closed");
		}
		return *this;
	}

	Vector2 GLWindow::GetCursorPos() const
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return Vector2(float(x), float(y));
	}

	Vector2 GLWindow::GetWindowPos() const
	{
		if (this->window != nullptr)
		{
			int x, y;
			glfwGetWindowPos(this->window, &x, &y);
			return Vector2(float(x), float(y));
		}
		return this->windowPosition;
	}

	bool GLWindow::IsKeyHeld(KeyCode key) const
	{
		return this->keyHeld[(size_t)key];
	}

	bool GLWindow::IsKeyPressed(KeyCode key) const
	{
		return this->keyPressed[(size_t)key];
	}

	bool GLWindow::IsKeyReleased(KeyCode key) const
	{
		return this->keyReleased[(size_t)key];
	}

	GLWindow& GLWindow::Create()
	{
		{
			MAKE_SCOPE_PROFILER("Window::Create");
			MAKE_SCOPE_TIMER("MxEngine::Window", "Window::Create");
			this->window = glfwCreateWindow(width, height, "", nullptr, nullptr);
			if (this->window == nullptr)
			{
				Logger::Instance().Error("GLFW", "glfw window was not created");
				return *this;
			}
			// window events
			SwitchContext();
			glfwSetWindowUserPointer(this->window, this);
			glfwSetKeyCallback(this->window, [](GLFWwindow* w, int key, int scancode, int action, int mods)
				{
					if (action == GLFW_REPEAT) return;

					GLWindow& window = *(GLWindow*)glfwGetWindowUserPointer(w);
					if ((size_t)key >= 350) return; // TODO: handle all key input
					window.keyPressed[(size_t)key] = (action == GLFW_PRESS);
					window.keyReleased[(size_t)key] = (action == GLFW_RELEASE);
					window.keyHeld[(size_t)key] = (action == GLFW_PRESS);
				});
			glfwSetCursorPosCallback(this->window, [](GLFWwindow* w, double x, double y)
				{
					GLWindow& window = *(GLWindow*)glfwGetWindowUserPointer(w);
					window.cursorPosition = { float(x), float(y) };
				});
			glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height)
				{
					glViewport(0, 0, width, height);
				});
		}
		auto& module = Graphics::Instance()->GetGraphicModule();
		module.OnWindowCreate(this->GetNativeHandler());

		UseTitle(this->title);
		UseCursorMode(this->cursorMode);
		UsePosition(this->windowPosition.x, this->windowPosition.y);
		UseCursorPos(this->cursorPosition);
		Logger::Instance().Debug("MxEngine::Window", "window initialized");
		return *this;
	}

	GLWindow& GLWindow::SwitchContext()
	{
		glfwMakeContextCurrent(this->window);
		return *this;
	}

	std::array<int, 3> ProfileType =
	{
		GLFW_OPENGL_ANY_PROFILE,
		GLFW_OPENGL_COMPAT_PROFILE,
		GLFW_OPENGL_CORE_PROFILE,
	};

	GLWindow& GLWindow::UseProfile(int majorVersion, int minorVersion, Profile profile)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, ProfileType[(int)profile]);
		return *this;
	}

	GLWindow& GLWindow::UseDebugging(bool value)
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, value);
		Logger::Instance().Debug("OpenGL::Window", "debug context was set to: " + (std::string)BOOL_STRING(value));
		return *this;
	}

	GLWindow& GLWindow::UseDoubleBuffering(bool value)
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, value);
		Logger::Instance().Debug("OpenGL::Window", "doublebuffering was set to: " + (std::string)BOOL_STRING(value));
		return *this;
	}

	std::array<int, 3> CursorType =
	{
		GLFW_CURSOR_NORMAL,
		GLFW_CURSOR_HIDDEN,
		GLFW_CURSOR_DISABLED,
	};

	GLWindow& GLWindow::UseCursorMode(CursorMode cursor)
	{
		this->cursorMode = cursor;
		if (window != nullptr)
			glfwSetInputMode(window, GLFW_CURSOR, CursorType[(int)cursor]);
		return *this;
	}

	GLWindow& GLWindow::UseCursorPos(const Vector2& pos)
	{
		this->cursorPosition = pos;
		if (this->window != nullptr)
		{
			glfwSetCursorPos(this->window, pos.x, pos.y);
		}
		return *this;
	}

	GLWindow& GLWindow::UseTitle(const std::string& title)
	{
		this->title = title;
		if (this->window != nullptr)
			glfwSetWindowTitle(window, title.c_str());
		return *this;
	}

	GLWindow& GLWindow::UsePosition(float xpos, float ypos)
	{
		this->windowPosition = { xpos, ypos };
		if(window != nullptr)
			glfwSetWindowPos(window, int(xpos), int(ypos));
		return *this;
	}

	GLWindow& GLWindow::UseSize(int width, int height)
	{
		this->width = width;
		this->height = height;
		if (window != nullptr)
			glfwSetWindowSize(this->window, width, height);
		return *this;
	}

	GLWindow& GLWindow::UseEventDispatcher(AppEventDispatcher* dispatcher)
	{
		this->dispatcher = dispatcher;
		return *this;
	}

	CursorMode GLWindow::GetCursorMode() const
	{
		return this->cursorMode;
	}

	GLWindow::~GLWindow()
	{
		this->Destroy();
	}

	int GLWindow::GetHeight() const
	{
		if (this->window == nullptr)
			return this->height;

		int width, height;
		glfwGetWindowSize(this->window, &width, &height);
		return height;
	}
}