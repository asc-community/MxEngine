#include "Window.h"
#include "Utilities/Logger/Logger.h"
#include "Core/OpenGL/GLInitializer/GLInitializer.h"
#include <GLFW/glfw3.h>
#include "Utilities/Time/Time.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace MomoEngine
{
	Window::Window(int width, int height)
		: width(width), height(height), window(nullptr), cursorMode(CursorMode::NORMAL), windowPosition{width, height}
	{
		GLInitilizer::Instance().Init();
		Logger::Instance().Debug("MomoEngine::Window", "window object created");
	}

	int Window::GetWidth() const
	{
		return this->width;
	}

	bool Window::IsOpen() const
	{
		if (this->window == nullptr)
		{
			Logger::Instance().Error("MomoEngine::Window", "Window::Create() was not called");
			return false;
		}
		bool isOpen = !glfwWindowShouldClose(this->window);

		GLInitilizer::Instance().OnWindowTick(this->window);

		return isOpen;
	}

	TimeStep Window::GetTime() const
	{
		return Time::Current();
	}

	void Window::PullEvents() const
	{
		glfwPollEvents();
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

	Position Window::GetCursorPos() const
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { int(x), int(y) };
	}

	Position Window::GetWindowPos() const
	{
		return this->windowPosition;
	}

	bool Window::IsKeyHolded(KeyCode key) const
	{
		return glfwGetKey(window, (int)key) == GLFW_PRESS;
	}

	bool Window::IsKeyPressed(KeyCode key) const
	{
		bool state = IsKeyHolded(key); // pressed now
		bool result = this->keyStates[(size_t)key] != state;
		this->keyStates[(size_t)key] = state;
		return result && state;
	}

	bool Window::IsKeyReleased(KeyCode key) const
	{
		bool state = !IsKeyHolded(key); // released now
		bool result = this->keyStates[(size_t)key] != state;
		this->keyStates[(size_t)key] = state;
		return result && state;
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
		SwitchContext();
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

	Window& Window::UseCursorPos(Position pos)
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

	Window& Window::UsePosition(int xpos, int ypos)
	{
		this->windowPosition = { xpos, ypos };
		if(window != nullptr)
			glfwSetWindowPos(window, xpos, ypos);
		return *this;
	}

	Window& Window::UseSize(int width, int height)
	{
		glfwSetWindowSize(this->window, width, height);
		return *this;
	}

	CursorMode Window::GetCursorMode() const
	{
		return this->cursorMode;
	}

	Window::~Window()
	{
		this->Close();
		glfwDestroyWindow(this->window);
		Logger::Instance().Debug("MomoEngine::Window", "window destroyed");
	}

	int Window::GetHeight() const
	{
		return this->height;
	}
}