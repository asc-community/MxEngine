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

#include "GraphicModule.h"
#include "Core/Config/GlobalConfig.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/ImGui/ImGuiBase.h"
#include "Utilities/FileSystem/FileManager.h"

#include <string>

namespace MxEngine
{
	MxString GetOpenGLShaderVersion()
	{
		return "#version " + ToMxString(GlobalConfig::GetGraphicAPIMajorVersion() * 100 + GlobalConfig::GetGraphicAPIMinorVersion() * 10);
	}

	void ImGuiStyleColorsMxEngineCustom();

	void InitializeGLFW()
	{
		MAKE_SCOPE_PROFILER("OpenGL::InitGLFW");
		MAKE_SCOPE_TIMER("MxEngine::GLGraphicModule", "OpenGL::InitGLFW");
		glfwSetErrorCallback([](int errorCode, const char* errorMessage)
			{
				MXLOG_ERROR("OpenGL::InitGLFW", errorMessage);
			});
		if (!glfwInit())
		{
			MXLOG_FATAL("OpenGL::InitGLFW", "OpenGL init failed");
			return;
		}
	}

	void InitializeImGui(void* window)
	{
		MAKE_SCOPE_PROFILER("ImGui::Init");
		MAKE_SCOPE_TIMER("MxEngine::GLGraphicModule", "ImGui::Init()");

		auto context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);

		auto& imguiIO = ImGui::GetIO();
		imguiIO.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
		imguiIO.ConfigDockingAlwaysTabBar = true;

		ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(window), true);
		ImGui_ImplOpenGL3_Init(GetOpenGLShaderVersion().c_str());

		switch (GlobalConfig::GetEditorStyle())
		{
		case EditorStyle::CLASSIC:
			ImGui::StyleColorsClassic();
			break;
		case EditorStyle::DARK:
			ImGui::StyleColorsDark();
			break;
		case EditorStyle::LIGHT:
			ImGui::StyleColorsLight();
			break;
		case EditorStyle::MXENGINE:
			ImGuiStyleColorsMxEngineCustom();
			break;
		}
	}

	void InitializeGLEW(void* window)
	{
		MAKE_SCOPE_PROFILER("OpenGL::InitGLEW");
		MAKE_SCOPE_TIMER("MxEngine::GLGraphicModule", "OpenGL::InitGLEW()");
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			MXLOG_FATAL("OpenGL::InitGLEW", "OpenGL init failed");
		}
		else
		{
			MXLOG_INFO("OpenGL::InitGLEW", "OpenGL version: " + MxString((char*)glGetString(GL_VERSION)));
		}
	}

	void InitializeDebug(void* window)
	{
		GLint flags;
		GLCALL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			GLCALL(glEnable(GL_DEBUG_OUTPUT));
			GLCALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
			GLCALL(glDebugMessageCallback(PrintDebugInformation, nullptr));
			GLCALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));
		}
	}

	void GraphicModule::Init()
	{
		InitializeGLFW();
	}

	void* GraphicModule::GetImpl()
	{
		return (void*)glfwGetCurrentContext();
	}

	void GraphicModule::Clone(void* context)
	{
		
		glfwMakeContextCurrent((GLFWwindow*)context);
	}

	void GraphicModule::OnWindowCreate(WindowHandle window)
	{
		InitializeGLEW(window);
		InitializeImGui(window);
		InitializeDebug(window);
	}

	void GraphicModule::OnWindowUpdate(WindowHandle window)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GraphicModule::OnWindowDestroy(WindowHandle window)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	void GraphicModule::OnRenderDraw()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GraphicModule::Destroy()
	{
		if(ImGui::GetCurrentContext() != nullptr)
			ImGui::DestroyContext();
		glfwTerminate();
	}

	void ImGuiStyleColorsMxEngineCustom()
	{
		constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
		{
			return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
		};

		auto& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		const ImVec4 bgColor           = ColorFromBytes(37, 37, 38);
		const ImVec4 lightBgColor      = ColorFromBytes(82, 82, 85);
		const ImVec4 veryLightBgColor  = ColorFromBytes(90, 90, 95);

		const ImVec4 panelColor        = ColorFromBytes(51, 51, 55);
		const ImVec4 panelHoverColor   = ColorFromBytes(29, 151, 236);
		const ImVec4 panelActiveColor  = ColorFromBytes(0, 119, 200);

		const ImVec4 textColor         = ColorFromBytes(255, 255, 255);
		const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
		const ImVec4 borderColor       = ColorFromBytes(78, 78, 78);

		colors[ImGuiCol_Text]                 = textColor;
		colors[ImGuiCol_TextDisabled]         = textDisabledColor;
		colors[ImGuiCol_TextSelectedBg]       = panelActiveColor;
		colors[ImGuiCol_WindowBg]             = bgColor;
		colors[ImGuiCol_ChildBg]              = bgColor;
		colors[ImGuiCol_PopupBg]              = bgColor;
		colors[ImGuiCol_Border]               = borderColor;
		colors[ImGuiCol_BorderShadow]         = borderColor;
		colors[ImGuiCol_FrameBg]              = panelColor;
		colors[ImGuiCol_FrameBgHovered]       = panelHoverColor;
		colors[ImGuiCol_FrameBgActive]        = panelActiveColor;
		colors[ImGuiCol_TitleBg]              = bgColor;
		colors[ImGuiCol_TitleBgActive]        = bgColor;
		colors[ImGuiCol_TitleBgCollapsed]     = bgColor;
		colors[ImGuiCol_MenuBarBg]            = panelColor;
		colors[ImGuiCol_ScrollbarBg]          = panelColor;
		colors[ImGuiCol_ScrollbarGrab]        = lightBgColor;
		colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
		colors[ImGuiCol_ScrollbarGrabActive]  = veryLightBgColor;
		colors[ImGuiCol_CheckMark]            = panelActiveColor;
		colors[ImGuiCol_SliderGrab]           = panelHoverColor;
		colors[ImGuiCol_SliderGrabActive]     = panelActiveColor;
		colors[ImGuiCol_Button]               = panelColor;
		colors[ImGuiCol_ButtonHovered]        = panelHoverColor;
		colors[ImGuiCol_ButtonActive]         = panelHoverColor;
		colors[ImGuiCol_Header]               = panelColor;
		colors[ImGuiCol_HeaderHovered]        = panelHoverColor;
		colors[ImGuiCol_HeaderActive]         = panelActiveColor;
		colors[ImGuiCol_Separator]            = borderColor;
		colors[ImGuiCol_SeparatorHovered]     = borderColor;
		colors[ImGuiCol_SeparatorActive]      = borderColor;
		colors[ImGuiCol_ResizeGrip]           = bgColor;
		colors[ImGuiCol_ResizeGripHovered]    = panelColor;
		colors[ImGuiCol_ResizeGripActive]     = lightBgColor;
		colors[ImGuiCol_PlotLines]            = panelActiveColor;
		colors[ImGuiCol_PlotLinesHovered]     = panelHoverColor;
		colors[ImGuiCol_PlotHistogram]        = panelActiveColor;
		colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
		colors[ImGuiCol_ModalWindowDarkening] = bgColor;
		colors[ImGuiCol_DragDropTarget]       = bgColor;
		colors[ImGuiCol_NavHighlight]         = bgColor;
		colors[ImGuiCol_DockingPreview]       = panelActiveColor;
		colors[ImGuiCol_Tab]                  = bgColor;
		colors[ImGuiCol_TabActive]            = panelActiveColor;
		colors[ImGuiCol_TabUnfocused]         = bgColor;
		colors[ImGuiCol_TabUnfocusedActive]   = panelActiveColor;
		colors[ImGuiCol_TabHovered]           = panelHoverColor;

		style.WindowRounding    = 0.0f;
		style.ChildRounding     = 0.0f;
		style.FrameRounding     = 0.0f;
		style.GrabRounding      = 0.0f;
		style.PopupRounding     = 0.0f;
		style.ScrollbarRounding = 0.0f;
		style.TabRounding       = 0.0f;
	}
}