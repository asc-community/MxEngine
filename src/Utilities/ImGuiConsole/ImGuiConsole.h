#pragma once

#include "imgui/imgui.h"
#include "Utilities/Memory/Memory.h"

namespace chaiscript { class ChaiScript; }

// modified console from ImGui example
namespace MomoEngine
{
	class ImGuiConsole
	{
		chaiscript::ChaiScript* chai = nullptr;

		char                  InputBuf[256];
		ImVector<char*>       Items;
		ImVector<char*>       History;
		int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
		ImVec2                size;
		ImGuiTextFilter       Filter;
		bool                  AutoScroll;
		bool                  ScrollToBottom;

		void ExecCommand(const char* command_line);
		void PrintHistory();
		int TextEditCallback(ImGuiInputTextCallbackData* data);

		friend int TextEditCallbackStub(ImGuiInputTextCallbackData*);
	public:
		ImGuiConsole();
		ImGuiConsole(const ImGuiConsole&) = delete;
		~ImGuiConsole();

		void SetChaiScriptObject(chaiscript::ChaiScript* chai);
		void ClearLog();
		void PrintLog(const char* fmt, ...);
		void Draw(const char* title);
		ImVec2 GetSize() const;
		void SetSize(ImVec2 size);
	};
}