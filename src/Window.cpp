#include "Window.h"
#include "Logger.h"
#include <GLFW/glfw3.h>

namespace MomoEngine
{
	Window::Window(int width, int height)
		: width(width), height(height), window(nullptr), cursorMode(CursorMode::NORMAL)
	{
		if (!glfwInit())
		{
			Logger::Get().Error("glfw", "glfw init failed");
			return;
		}
	}

	int Window::GetWidth() const
	{
		return this->width;
	}

	bool Window::IsOpen() const
	{
		return !glfwWindowShouldClose(window);
	}

	float Window::GetTime() const
	{
		return static_cast<float>(glfwGetTime());
	}

	void Window::PullEvents() const
	{
		glfwPollEvents();
	}

	Window::Position Window::GetCursorPos() const
	{
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { float(x), float(y) };
	}

	Window::Position Window::GetWindowPos() const
	{
		return { (float)this->xpos, (float)this->ypos };
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
			Logger::Get().Error("glfw", "glfw window was not created");
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		UseTitle(this->title);
		UseCursorMode(this->cursorMode);
		UsePosition(this->xpos, this->ypos);

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

	Window& Window::UseTitle(const std::string& title)
	{
		this->title = title;
		if (window != nullptr)
			glfwSetWindowTitle(window, title.c_str());
		return *this;
	}

	Window& Window::UsePosition(int xpos, int ypos)
	{
		this->xpos = xpos;
		this->ypos = ypos;
		if(window != nullptr)
			glfwSetWindowPos(window, xpos, ypos);
		return *this;
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	int Window::GetHeight() const
	{
		return this->height;
	}
}