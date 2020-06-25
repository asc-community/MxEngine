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

#include "Window.h"

#include "Utilities/Logger/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Event/Events/MouseEvent.h"
#include "Core/Event/Events/WindowResizeEvent.h"
#include "Platform/Modules/GraphicModule.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Format/Format.h"

#include <array>

namespace MxEngine
{
	void Window::Destroy()
	{
		if (this->window != nullptr)
		{
			this->Close();
			glfwDestroyWindow(this->window);
			Logger::Instance().Debug("MxEngine::Window", "window destroyed");
		}
	}

	void Window::Move(Window&& other)
	{
		this->title = std::move(other.title);
		this->width = other.width;
		this->height = other.height;
		this->window = other.window;
		this->dispatcher = other.dispatcher;
		this->keyHeld = other.keyHeld;
		this->keyPressed = other.keyPressed;
		this->keyReleased = other.keyReleased;
		this->cursorMode = other.cursorMode;
		this->windowPosition = other.windowPosition;
		this->cursorPosition = other.cursorPosition;

		other.width = 0;
		other.height = 0;
		other.window = nullptr;
		other.dispatcher = nullptr;
		other.cursorMode = CursorMode::NORMAL;
		other.windowPosition = MakeVector2(0.0f);
		other.cursorPosition = MakeVector2(0.0f);
		other.keyHeld.reset();
		other.keyReleased.reset();
		other.keyPressed.reset();
	}

	Window::Window(int width, int height, const MxString& title)
		: title(title), width(width), height(height)
	{		
		Logger::Instance().Debug("MxEngine::Window", "window object created");
	}

    Window::Window(Window&& other) noexcept
    {
		this->Move(std::move(other));
    }

	int Window::GetWidth() const
	{
		if (this->window == nullptr)
			return this->width;
		
		int width, height;
		glfwGetWindowSize(this->window, &width, &height);
		return width;
	}

	Window::WindowHandle Window::GetNativeHandle()
	{
		return this->window;
	}

	AppEventDispatcher& Window::GetEventDispatcher()
	{
		return *this->dispatcher;
	}

	bool Window::IsCreated() const
	{
		return this->window != nullptr;
	}

	bool Window::IsOpen() const
	{
		if (this->window == nullptr)
		{
			Logger::Instance().Warning("MxEngine::Window", "window was not created while calling Window::IsOpen");
			return false;
		}
		bool isOpen = !glfwWindowShouldClose(this->window);

		return isOpen;
	}

	void Window::PullEvents() const
	{		
		MAKE_SCOPE_PROFILER("Window::PullEvents");
		this->keyPressed.reset();
		this->keyReleased.reset();

		glfwPollEvents();

		if (this->doubleBuffer)
			glfwSwapBuffers(this->window);

		if (this->dispatcher != nullptr)
		{
			auto keyEvent = MakeUnique<KeyEvent>(&this->keyHeld, &this->keyPressed, &this->keyReleased);
			this->dispatcher->AddEvent(std::move(keyEvent));
			auto mouseMoveEvent = MakeUnique<MouseMoveEvent>(this->cursorPosition.x, this->cursorPosition.y);
			this->dispatcher->AddEvent(std::move(mouseMoveEvent));
		}
	}

	void Window::OnUpdate()
	{
		GraphicModule::OnWindowUpdate(this->GetNativeHandle());

		if (this->dispatcher != nullptr)
		{
			Vector2 currentSize(this->GetWidth(), this->GetHeight());
			Vector2 prevSize(this->width, this->height);
			if (currentSize != prevSize)
			{
				this->dispatcher->AddEvent(MakeUnique<WindowResizeEvent>(prevSize, currentSize));
				this->width =  (int)currentSize.x;
				this->height = (int)currentSize.y;
			}
		}
	}

	Window& Window::Close()
	{
		if (this->window != nullptr && IsOpen())
		{
			GraphicModule::OnWindowDestroy(this->GetNativeHandle());

			glfwSetWindowShouldClose(this->window, (int)true);
			Logger::Instance().Debug("MxEngine::Window", "window closed");
		}
		return *this;
	}

	Vector2 Window::GetCursorPosition() const
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return Vector2(float(x), float(y));
	}

	Vector2 Window::GetWindowPosition() const
	{
		if (this->window != nullptr)
		{
			int x, y;
			glfwGetWindowPos(this->window, &x, &y);
			return Vector2(float(x), float(y));
		}
		return this->windowPosition;
	}

	bool Window::IsKeyHeld(KeyCode key) const
	{
		return key != KeyCode::UNKNOWN && this->keyHeld[(size_t)key];
	}

	bool Window::IsKeyPressed(KeyCode key) const
	{
		return key != KeyCode::UNKNOWN && this->keyPressed[(size_t)key];
	}

	bool Window::IsKeyReleased(KeyCode key) const
	{
		return key != KeyCode::UNKNOWN && this->keyReleased[(size_t)key];
	}

	Window& Window::Create()
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

					Window& window = *(Window*)glfwGetWindowUserPointer(w);
					if ((size_t)key >= 350) return; // TODO: handle all key input
					window.keyPressed[(size_t)key] = (action == GLFW_PRESS);
					window.keyReleased[(size_t)key] = (action == GLFW_RELEASE);
					window.keyHeld[(size_t)key] = (action == GLFW_PRESS);
				});
			glfwSetCursorPosCallback(this->window, [](GLFWwindow* w, double x, double y)
				{
					Window& window = *(Window*)glfwGetWindowUserPointer(w);
					window.cursorPosition = { float(x), float(y) };
				});
			glfwSetWindowSizeCallback(window, [](GLFWwindow* w, int width, int height)
				{
					glViewport(0, 0, width, height);
				});
		}
		GraphicModule::OnWindowCreate(this->GetNativeHandle());

		UseTitle(this->title);
		UseCursorMode(this->cursorMode);
		UseWindowPosition((int)this->windowPosition.x, (int)this->windowPosition.y);
		UseCursorPosition(this->cursorPosition);
		Logger::Instance().Debug("MxEngine::Window", "window initialized");
		return *this;
	}

	Window& Window::SwitchContext()
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

	Window& Window::UseProfile(int majorVersion, int minorVersion, Profile profile)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, ProfileType[(int)profile]);
		return *this;
	}

	Window& Window::UseDebugging(bool value)
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, value);
		Logger::Instance().Debug("OpenGL::Window", "debug context was set to: " + (MxString)BOOL_STRING(value));
		return *this;
	}

	Window& Window::UseDoubleBuffering(bool value)
	{
		this->doubleBuffer = value;
		glfwWindowHint(GLFW_DOUBLEBUFFER, value);
		Logger::Instance().Debug("OpenGL::Window", "doublebuffering was set to: " + (MxString)BOOL_STRING(value));
		return *this;
	}

	std::array<int, 3> CursorType =
	{
		GLFW_CURSOR_NORMAL,
		GLFW_CURSOR_HIDDEN,
		GLFW_CURSOR_DISABLED,
	};

	Window& Window::UseCursorMode(CursorMode cursor)
	{
		this->cursorMode = cursor;
		if (window != nullptr)
			glfwSetInputMode(window, GLFW_CURSOR, CursorType[(int)cursor]);
		return *this;
	}

	Window& Window::UseCursorPosition(const Vector2& pos)
	{
		this->cursorPosition = pos;
		if (this->window != nullptr)
		{
			glfwSetCursorPos(this->window, pos.x, pos.y);
		}
		return *this;
	}

	Window& Window::UseTitle(const MxString& title)
	{
		this->title = title;
		if (this->window != nullptr)
			glfwSetWindowTitle(window, title.c_str());
		return *this;
	}

	Window& Window::UseWindowPosition(int xpos, int ypos)
	{
		this->windowPosition = { (float)xpos, (float)ypos };
		if(window != nullptr)
			glfwSetWindowPos(window, xpos, ypos);
		return *this;
	}

	Window& Window::UseWindowSize(int width, int height)
	{
		if (window != nullptr)
		{
			glfwSetWindowSize(this->window, width, height);
			if (this->dispatcher != nullptr)
				this->dispatcher->AddEvent(
					MakeUnique<WindowResizeEvent>(MakeVector2((float)this->width, (float)this->height), MakeVector2((float)width, (float)height)));
		}
		this->width = width;
		this->height = height;
		return *this;
	}

	Window& Window::UseEventDispatcher(AppEventDispatcher* dispatcher)
	{
		this->dispatcher = dispatcher;
		return *this;
	}

	CursorMode Window::GetCursorMode() const
	{
		return this->cursorMode;
	}

    const MxString& Window::GetTitle() const
    {
		return this->title;
    }

	Window& Window::operator=(Window&& other) noexcept
	{
		this->Move(std::move(other));
		return *this;
	}

	Window::~Window()
	{
		this->Destroy();
	}

	int Window::GetHeight() const
	{
		if (this->window == nullptr)
			return this->height;

		int width, height;
		glfwGetWindowSize(this->window, &width, &height);
		return height;
	}
}