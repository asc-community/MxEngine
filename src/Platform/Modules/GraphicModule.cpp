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
#include "Core/Application/GlobalConfig.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/ImGui/ImGuiBase.h"

#include <string>

namespace MxEngine
{
	MxString GetOpenGLShaderVersion()
	{
		return "#version " + ToMxString(GlobalConfig::GetGraphicAPIMajorVersion() * 100 + GlobalConfig::GetGraphicAPIMinorVersion() * 10);
	}

	void InitializeGLFW()
	{
		MAKE_SCOPE_PROFILER("OpenGL::InitGLFW");
		MAKE_SCOPE_TIMER("MxEngine::GLGraphicModule", "OpenGL::InitGLFW");
		glfwSetErrorCallback([](int errorCode, const char* errorMessage)
			{
				MXLOG_FATAL("OpenGL::InitGLFW", errorMessage);
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
		ImGui::StyleColorsDark();

		auto& imguiIO = ImGui::GetIO();
		imguiIO.ConfigFlags |= ImGuiConfigFlags_::ImGuiConfigFlags_DockingEnable;
		imguiIO.ConfigDockingAlwaysTabBar = true;

		ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(window), true);
		ImGui_ImplOpenGL3_Init(GetOpenGLShaderVersion().c_str());
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
}