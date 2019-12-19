#include "GLInitializer.h"
#include "../Core/OpenGL/GLUtils/GLUtils.h"
#include "../Utilities/Logger/Logger.h"

MomoEngine::GLInitilizerImpl::GLInitilizerImpl()
{
	if (!glfwInit())
	{
		Logger::Instance().Error("glfw", "glfw init failed");
		return;
	}
}

MomoEngine::GLInitilizerImpl::~GLInitilizerImpl()
{
	glfwTerminate();
}

void MomoEngine::GLInitilizerImpl::IntializeWindow()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		Logger::Instance().Error("glew", "glew init failed");
	}
	else
	{
		Logger::Instance().Debug("glew", "OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	}
}

void MomoEngine::GLInitilizerImpl::OnWindowCreate() { }
