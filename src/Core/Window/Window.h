#pragma once

#include <string>
#include <bitset>

#include "Utilities/Time/Time.h"
#include "Core/Interfaces/IEvent.h"
#include "Utilities/Math/Math.h"
#include "Core/Event/KeyEvent.h"

struct GLFWwindow; // from glfw header file

namespace MomoEngine
{
	// from glfw header file
	enum class Profile
	{
		ANY = 0,
		COMPAT = 0x00032002,
		CORE = 0x00032001,
	};

	// from glfw header file
	enum class CursorMode
	{
		NORMAL = 0x00034001,
		HIDDEN = 0x00034002,
		DISABLED = 0x00034003,
	};

	class Window
	{
	private:
		std::string title;
		GLFWwindow* window;
		int width, height;
		AppEventDispatcher* dispatcher;
		mutable std::bitset<350> keyHeld;
		mutable std::bitset<350> keyPressed;
		mutable std::bitset<350> keyReleased;
		CursorMode cursorMode;
		Vector2 windowPosition{ 0.0f, 0.0f };
		Vector2 cursorPosition{ 0.0f, 0.0f };
	public:
		Window(int width, int height);
		Window(const Window&) = delete;
		Window(Window&& window) noexcept;
		Window& operator=(Window&& window) noexcept;
		Window& operator=(const Window&) = delete;
		~Window();

		int GetHeight() const;
		int GetWidth() const;
		bool IsOpen() const;
		TimeStep GetTime() const;
		void PullEvents() const;
		void OnUpdate();

		Vector2 GetCursorPos() const;
		Vector2 GetWindowPos() const;
		bool IsKeyHeld(KeyCode key) const;
		bool IsKeyPressed(KeyCode key) const;
		bool IsKeyReleased(KeyCode key) const;
		GLFWwindow* GetNativeHandler();

		Window& Create();
		Window& Close();
		Window& SwitchContext();
		Window& UseProfile(int majorVersion, int minorVersion, Profile profile);
		Window& UseSampling(int samples);
		Window& UseDoubleBuffering(bool value = true);
		Window& UseCursorMode(CursorMode cursor);
		Window& UseCursorPos(const Vector2& pos);
		Window& UseTitle(const std::string& title);
		Window& UsePosition(float xpos, float ypos);
		Window& UseSize(int width, int height);
		Window& UseEventDispatcher(AppEventDispatcher* dispatcher);

		CursorMode GetCursorMode() const;
	};
}