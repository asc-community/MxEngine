#include "GLInitializer.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Time/Time.h"

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
}

MomoEngine::GLInitilizerImpl::~GLInitilizerImpl()
{
	glfwTerminate();
}

void MomoEngine::GLInitilizerImpl::IntializeWindow()
{
	TimeStep initStart = (TimeStep)glfwGetTime();
	GLenum err = glewInit();
	TimeStep initEnd = (TimeStep)glfwGetTime();
	if (err != GLEW_OK)
	{
		Logger::Instance().Error("GLEW", "glew init failed");
	}
	else
	{
		Logger::Instance().Debug("GLEW", "glew init done in " + BeautifyTime(initEnd - initStart));
		Logger::Instance().Debug("GLEW", "OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	}
}

void MomoEngine::GLInitilizerImpl::OnWindowCreate() { }
