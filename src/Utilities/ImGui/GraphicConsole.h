#pragma once

#include "imgui/imgui.h"

#include <functional>

// modified console from ImGui example
namespace MomoEngine
{
	class GraphicConsole
	{
		char                  InputBuf[256];
		ImVector<char*>       Items;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImVec2                size;
		ImGuiTextFilter       Filter;
		bool                  AutoScroll;
		bool                  ScrollToBottom;

		void ExecCommand(const char* command_line);
		int TextEditCallback(ImGuiInputTextCallbackData* data);
        using EventCallback = std::function<void(const char*)>;
        EventCallback eventCallback = nullptr;

		friend int TextEditCallbackStub(ImGuiInputTextCallbackData*);
	public:
		GraphicConsole();
		GraphicConsole(const GraphicConsole&) = delete;
		~GraphicConsole();

        void SetEventCallback(EventCallback callback);
        void PrintHistory();
		void ClearLog();
		void PrintLog(const char* fmt, ...);
		void Draw(const char* title);
		ImVec2 GetSize() const;
		void SetSize(ImVec2 size);
	};
}