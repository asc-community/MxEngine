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
        {
            MAKE_SCOPE_PROFILER("ImGui::CreateContext");
            auto context = ImGui::CreateContext();
            ImGui::SetCurrentContext(context);
            ImGui::StyleColorsDark();
            Logger::Instance().Debug("ImGui", "ImGui context created");
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
        ImGui::DestroyContext();
        glfwTerminate();
    }
}