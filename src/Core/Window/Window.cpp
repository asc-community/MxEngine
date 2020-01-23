#include "Window.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Event/KeyEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/OpenGL/GLInitializer/GLInitializer.h"
#include "Utilities/Time/Time.h"
#include <imgui/imgui.h>

#include <GLFW/glfw3.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <algorithm>

namespace MomoEngine
{
	Window::Window(int width, int height)
		: width(width), height(height), window(nullptr), cursorMode(CursorMode::NORMAL), windowPosition{float(width), float(height)}, dispatcher(nullptr)
	{
		GLInitilizer::Instance().Init();
		Logger::Instance().Debug("MomoEngine::Window", "window object created");
	}

	Window::Window(Window&& window) noexcept
		: title(std::move(window.title)), window(window.window), width(window.width),
		height(window.height), windowPosition(window.windowPosition), cursorMode(window.cursorMode),
		cursorPosition(window.cursorPosition), keyHeld(std::move(window.keyHeld)), dispatcher(window.dispatcher)
	{
		window.window = nullptr;
		window.dispatcher = nullptr;
	}

	Window& Window::operator=(Window&& window) noexcept
	{
		this->title          = std::move(window.title);
		this->window         = window.window;
		this->width          = window.width,
		this->height         = window.height;
		this->windowPosition = window.windowPosition;
		this->cursorMode     = window.cursorMode;
		this->cursorPosition = window.cursorPosition;
		this->keyHeld      = window.keyHeld;
		this->dispatcher     = window.dispatcher;
		window.window        = nullptr;
		window.dispatcher    = nullptr;

		return *this;
	}

	int Window::GetWidth() const
	{
		if (this->window == nullptr)
			return this->width;
		
		int width, height;
		glfwGetWindowSize(this->window, &width, &height);
		return width;
	}

	bool Window::IsOpen() const
	{
		if (this->window == nullptr)
		{
			Logger::Instance().Error("MomoEngine::Window", "Window::Create() was not called");
			return false;
		}
		bool isOpen = !glfwWindowShouldClose(this->window);

		return isOpen;
	}

	TimeStep Window::GetTime() const
	{
		return Time::Current();
	}

	void Window::PullEvents() const
	{		
		this->keyPressed.reset();
		this->keyReleased.reset();

		glfwPollEvents();

		auto keyEvent = MakeUnique<KeyEvent>(&this->keyHeld, &this->keyPressed, &this->keyReleased);
		this->dispatcher->AddEvent(std::move(keyEvent));
		auto mouseMoveEvent = MakeUnique<MouseMoveEvent>(this->cursorPosition.x, this->cursorPosition.y);
		this->dispatcher->AddEvent(std::move(mouseMoveEvent));
	}

	void Window::OnUpdate()
	{
		GLInitilizer::Instance().OnWindowTick(this->window);
	}

	Window& Window::Close()
	{
		if (this->window != nullptr && IsOpen())
		{
			GLInitilizer::Instance().OnWindowClose(this->window);
			glfwSetWindowShouldClose(this->window, true);
			Logger::Instance().Debug("MomoEngine::Window", "window closed");
		}
		return *this;
	}

	Vector2 Window::GetCursorPos() const
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return Vector2(float(x), float(y));
	}

	Vector2 Window::GetWindowPos() const
	{
		return this->windowPosition;
	}

	bool Window::IsKeyHeld(KeyCode key) const
	{
		return this->keyHeld[(size_t)key];
	}

	bool Window::IsKeyPressed(KeyCode key) const
	{
		return this->keyPressed[(size_t)key];
	}

	bool Window::IsKeyReleased(KeyCode key) const
	{
		return this->keyReleased[(size_t)key];
	}

	GLFWwindow* Window::GetNativeHandler()
	{
		return window;
	}

	Window& Window::Create()
	{
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
				window.keyPressed[(size_t)key] = (action == GLFW_PRESS);
				window.keyReleased[(size_t)key] = (action == GLFW_RELEASE);
				window.keyHeld[(size_t)key] = (action == GLFW_PRESS);
			});
		glfwSetCursorPosCallback(this->window, [](GLFWwindow* w, double x, double y)
			{
				Window& window = *(Window*)glfwGetWindowUserPointer(w);
				window.cursorPosition = { float(x), float(y) };
			});

		GLInitilizer::Instance().OnWindowCreate(this->window);

		UseTitle(this->title);
		UseCursorMode(this->cursorMode);
		UsePosition(this->windowPosition.x, this->windowPosition.y);
		UseCursorPos(this->cursorPosition);
		Logger::Instance().Debug("MomoEngine::Window", "window initialized");
		return *this;
	}

	Window& Window::SwitchContext()
	{
		glfwMakeContextCurrent(this->window);
		return *this;
	}

	Window& Window::UseProfile(int majorVersion, int minorVersion, Profile profile)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, (int)profile);
		return *this;
	}

	Window& Window::UseSampling(int samples)
	{
		glfwWindowHint(GLFW_SAMPLES, samples);
		return *this;
	}

	Window& Window::UseDoubleBuffering(bool value)
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, value);
		return *this;
	}

	Window& Window::UseCursorMode(CursorMode cursor)
	{
		this->cursorMode = cursor;
		if (window != nullptr)
			glfwSetInputMode(window, GLFW_CURSOR, (int)cursor);
		return *this;
	}

	Window& Window::UseCursorPos(const Vector2& pos)
	{
		this->cursorPosition = pos;
		if (this->window != nullptr)
		{
			glfwSetCursorPos(this->window, pos.x, pos.y);
		}
		return *this;
	}

	Window& Window::UseTitle(const std::string& title)
	{
		this->title = title;
		if (window != nullptr)
			glfwSetWindowTitle(window, title.c_str());
		return *this;
	}

	Window& Window::UsePosition(float xpos, float ypos)
	{
		this->windowPosition = { xpos, ypos };
		if(window != nullptr)
			glfwSetWindowPos(window, int(xpos), int(ypos));
		return *this;
	}

	Window& Window::UseSize(int width, int height)
	{
		glfwSetWindowSize(this->window, width, height);
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

	Window::~Window()
	{
		if (this->window != nullptr)
		{
			this->Close();
			glfwDestroyWindow(this->window);
			Logger::Instance().Debug("MomoEngine::Window", "window destroyed");
		}
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