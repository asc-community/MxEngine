#include "GLWindow.h"

#include "Utilities/Logger/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Memory/Memory.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

namespace MomoEngine
{
    void GLWindow::Destroy()
    {
        if (this->window != nullptr)
        {
            this->Close();
            glfwDestroyWindow(this->window);
            Logger::Instance().Debug("MomoEngine::Window", "window destroyed");
        }
    }

    GLWindow::GLWindow(int width, int height, const std::string& title)
		: title(title), width(width), height(height)
	{		
		Logger::Instance().Debug("MomoEngine::Window", "window object created");
	}

    GLWindow::GLWindow(GLWindow&& window) noexcept
		: title(std::move(window.title)), window(window.window), width(window.width),
		height(window.height), dispatcher(window.dispatcher), keyHeld(window.keyHeld),
		cursorMode(window.cursorMode), windowPosition(window.windowPosition), cursorPosition(window.cursorPosition)
	{
		window.window = nullptr;
		window.dispatcher = nullptr;
	}

	GLWindow& GLWindow::operator=(GLWindow&& window) noexcept
	{
		this->title          = std::move(window.title);
		this->window         = window.window;
		this->width          = window.width,
		this->height         = window.height;
		this->windowPosition = window.windowPosition;
		this->cursorMode     = window.cursorMode;
		this->cursorPosition = window.cursorPosition;
		this->keyHeld        = window.keyHeld;
		this->dispatcher     = window.dispatcher;
		window.window        = nullptr;
		window.dispatcher    = nullptr;

		return *this;
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

	bool GLWindow::IsOpen() const
	{
		if (this->window == nullptr)
		{
            Logger::Instance().Warning("MomoEngine::Window", "window was not created while calling Window::IsOpen");
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

		auto keyEvent = MakeUnique<KeyEvent>(&this->keyHeld, &this->keyPressed, &this->keyReleased);
		this->dispatcher->AddEvent(std::move(keyEvent));
		auto mouseMoveEvent = MakeUnique<MouseMoveEvent>(this->cursorPosition.x, this->cursorPosition.y);
		this->dispatcher->AddEvent(std::move(mouseMoveEvent));
	}

	void GLWindow::OnUpdate()
	{
        auto& module = Graphics::Instance()->GetGraphicModule();
        module.OnWindowUpdate(this->GetNativeHandler());
	}

	GLWindow& GLWindow::Close()
	{
		if (this->window != nullptr && IsOpen())
		{
            auto& module = Graphics::Instance()->GetGraphicModule();
            module.OnWindowDestroy(this->GetNativeHandler());

			glfwSetWindowShouldClose(this->window, true);
			Logger::Instance().Debug("MomoEngine::Window", "window closed");
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
                    window.keyPressed[(size_t)key] = (action == GLFW_PRESS);
                    window.keyReleased[(size_t)key] = (action == GLFW_RELEASE);
                    window.keyHeld[(size_t)key] = (action == GLFW_PRESS);
                });
            glfwSetCursorPosCallback(this->window, [](GLFWwindow* w, double x, double y)
                {
                    GLWindow& window = *(GLWindow*)glfwGetWindowUserPointer(w);
                    window.cursorPosition = { float(x), float(y) };
                });
            glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
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
		Logger::Instance().Debug("MomoEngine::Window", "window initialized");
		return *this;
	}

	GLWindow& GLWindow::SwitchContext()
	{
		glfwMakeContextCurrent(this->window);
		return *this;
	}

	GLWindow& GLWindow::UseProfile(int majorVersion, int minorVersion, Profile profile)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
		glfwWindowHint(GLFW_OPENGL_PROFILE, (int)profile);
		return *this;
	}

	GLWindow& GLWindow::UseSampling(int samples)
	{
		glfwWindowHint(GLFW_SAMPLES, samples);
		return *this;
	}

	GLWindow& GLWindow::UseDoubleBuffering(bool value)
	{
		glfwWindowHint(GLFW_DOUBLEBUFFER, value);
		return *this;
	}

	GLWindow& GLWindow::UseCursorMode(CursorMode cursor)
	{
		this->cursorMode = cursor;
		if (window != nullptr)
			glfwSetInputMode(window, GLFW_CURSOR, (int)cursor);
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