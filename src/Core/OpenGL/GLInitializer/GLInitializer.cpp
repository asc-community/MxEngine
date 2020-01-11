#include "GLInitializer.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Time/Time.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void OnGLFWErrorCallback(int errorCode, const char* errorMessage)
{
	MomoEngine::Logger::Instance().Error("GLFW", errorMessage);
}

MomoEngine::GLInitilizerImpl::GLInitilizerImpl()
{
	glfwSetErrorCallback(OnGLFWErrorCallback);
	if (!glfwInit())
	{
		Logger::Instance().Error("GLFW", "glfw init failed");
		return;
	}

	auto context = ImGui::CreateContext();
	ImGui::SetCurrentContext(context);
	Logger::Instance().Debug("ImGui", "ImGui context created");
}

MomoEngine::GLInitilizerImpl::~GLInitilizerImpl()
{
	ImGui::DestroyContext();
	glfwTerminate();
}

void MomoEngine::GLInitilizerImpl::OnWindowClose(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void MomoEngine::GLInitilizerImpl::OnWindowTick(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void MomoEngine::GLInitilizerImpl::OnWindowCreate(GLFWwindow* window)
{
	TimeStep initStart = Time::Current();
	GLenum err = glewInit();
	TimeStep initEnd = Time::Current();
	if (err != GLEW_OK)
	{
		Logger::Instance().Error("GLEW", "glew init failed");
	}
	else
	{
		Logger::Instance().Debug("GLEW", "glew init done in " + BeautifyTime(initEnd - initStart));
		Logger::Instance().Debug("GLEW", "OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	}

	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
	{
		GLCALL(glViewport(0, 0, width, height));
	});
	
	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window, true);

	GLCALL(std::string shaderVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	auto idx = shaderVersion.find('.');
	if (idx != shaderVersion.npos)
	{
		shaderVersion.erase(idx + 3, shaderVersion.size()); // 3.30 ... -> 3.30
		shaderVersion.erase(idx, idx); // 3.30 -> 330
	}
	shaderVersion = "#version " + shaderVersion; // 330 -> #version 330
	ImGui_ImplOpenGL3_Init(shaderVersion.c_str());
}
