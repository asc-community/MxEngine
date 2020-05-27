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

#pragma once

#include "Utilities/ImGui/ImGuiBase.h"

#include <functional>

namespace MxEngine
{
	/*!
	graphic console is modified console from ImGui demo example. It features text input, search, history and logging
	*/
	class GraphicConsole
	{
		char               InputBuf[256];
		ImVector<char*>	   Items;
		ImVector<char*>	   History;
		int				   HistoryPos;	// -1: new line, 0..History.size-1 browsing history.
		ImVec2			   size;
		ImGuiTextFilter	   Filter;
		bool			   AutoScroll;
		bool			   ScrollToBottom;

		/*!
		called internally by console. Executes command, forwarding it to the current script interpreter (see eventCallback documentation)
		\param command script to execute
		*/
		void ExecCommand(const char* command);
		/*!
		called internally by console. Executes callback code on text input
		\param data ImGui event data
		*/
		int TextEditCallback(ImGuiInputTextCallbackData* data);
		using EventCallback = std::function<void(const char*)>;
		/*!
		interpeter callback, called internally by ExecCommand function (is set by DeveloperConsole class)
		*/
		EventCallback eventCallback = nullptr;

		/*!
		called internally by console. this function is callback forwarder (acquires console object to forward event)
		*/
		friend int TextEditCallbackStub(ImGuiInputTextCallbackData*);
	public:
		/*!
		Constructs default console sized (450, 500)
		*/
		GraphicConsole();
		GraphicConsole(const GraphicConsole&) = delete;
		~GraphicConsole();

		/*!
		sets event callback which activates when text line is submitted by user
		\param callback functor to call on line input
		*/
		void SetEventCallback(EventCallback&& callback);
		/*!
		prints all unique lines inputted by user to console
		*/
		void PrintHistory();
		/*!
		clears console
		*/
		void ClearLog();
		/*!
		prints message in a C-style to a console windows
		\param fmt C-style formatting string
		\param args variadic argument list
		*/
		void PrintLog(const char* fmt, ...);
		/*!
		calls ImGui code to draw console in a new personal window
		\param title title of window in which console is drawn
		*/
		void Draw(const char* title);
		/*!
		size getter
		\returns console size in pixels
		*/
		ImVec2 GetSize() const;
		/*!
		size setter
		\param size new console size in pixels
		*/
		void SetSize(ImVec2 size);
	};
}