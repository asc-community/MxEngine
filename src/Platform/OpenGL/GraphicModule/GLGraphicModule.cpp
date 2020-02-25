// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "GLGraphicModule.h"
#include "Platform/OpenGL/GLUtilities/GLUtilities.h"

#include "Utilities/Logger/Logger.h"
#include "Utilities/Profiler/Profiler.h"

#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace MxEngine
{
	void GLGraphicModule::Init()
	{
		{
			MAKE_SCOPE_PROFILER("OpenGL::InitGLFW");
			MAKE_SCOPE_TIMER("MxEngine::GLGraphicModule", "OpenGL::InitGLFW");
			glfwSetErrorCallback([](int errorCode, const char* errorMessage)
				{
					Logger::Instance().Error("GLFW", errorMessage);
				});
			if (!glfwInit())
			{
				Logger::Instance().Error("GLFW", "glfw init failed");
				return;
			}
		}
	}

	void GLGraphicModule::OnWindowCreate(WindowHandler* window)
	{
		{
			MAKE_SCOPE_PROFILER("OpenGL::InitGLEW");
			MAKE_SCOPE_TIMER("MxEngine::GLGraphicModule", "OpenGL::InitGLEW()");
			GLenum err = glewInit();
			if (err != GLEW_OK)
			{
				Logger::Instance().Error("GLEW", "glew init failed");
			}
			else
			{
				Logger::Instance().Debug("GLEW", "OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
			}
		}

		{
			MAKE_SCOPE_PROFILER("ImGui::Init");
			MAKE_SCOPE_TIMER("MxEngine::GLGraphicModule", "ImGui::Init()");

			auto context = ImGui::CreateContext();
			ImGui::SetCurrentContext(context);
			ImGui::StyleColorsDark();

			ImGui_ImplGlfw_InitForOpenGL(reinterpret_cast<GLFWwindow*>(window), true);

			GLCALL(std::string shaderVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
			auto idx = shaderVersion.find('.');
			if (idx != shaderVersion.npos)
			{
				if (idx > 0) shaderVersion.erase(0, idx - 1); // ... 3.30 ... -> 3.30 ...
				shaderVersion.erase(idx + 3, shaderVersion.size()); // 3.30 ... -> 3.30
				shaderVersion.erase(idx, idx); // 3.30 -> 330
			}
			shaderVersion = "#version " + shaderVersion; // 330 -> #version 330
			ImGui_ImplOpenGL3_Init(shaderVersion.c_str());
		}
	}

	void GLGraphicModule::OnWindowUpdate(WindowHandler* window)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GLGraphicModule::OnWindowDestroy(WindowHandler* window)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

	void GLGraphicModule::OnRenderDraw()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void GLGraphicModule::Destroy()
	{
		if(ImGui::GetCurrentContext() != nullptr)
			ImGui::DestroyContext();
		glfwTerminate();
	}
}