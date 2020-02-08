#pragma once

#include <string>
#include <bitset>

#include "Utilities/Time/Time.h"
#include "Core/Interfaces/IEvent.h"
#include "Utilities/Math/Math.h"
#include "Core/Event/KeyEvent.h"
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

        void Destroy();
	public:
        GLWindow() = default;
		GLWindow(int width, int height, const std::string& title);
		GLWindow(const GLWindow&) = delete;
		GLWindow(GLWindow&& window) noexcept;
		GLWindow& operator=(GLWindow&& window) noexcept;
		GLWindow& operator=(const GLWindow&) = delete;
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
        virtual GLWindow& Create() override;
        virtual GLWindow& Close() override;
        virtual GLWindow& SwitchContext() override;
        virtual GLWindow& UseDoubleBuffering(bool value = true) override;
        virtual GLWindow& UseCursorMode(CursorMode cursor) override;
        virtual GLWindow& UseCursorPos(const Vector2& pos) override;
        virtual GLWindow& UseTitle(const std::string& title) override;
        virtual GLWindow& UsePosition(float xpos, float ypos) override;
        virtual GLWindow& UseSize(int width, int height) override;
        virtual GLWindow& UseEventDispatcher(AppEventDispatcher* dispatcher) override;
        virtual GLWindow& UseProfile(int majorVersion, int minorVersion, Profile profile) override;
        virtual GLWindow& UseSampling(int samples) override;
    };
}