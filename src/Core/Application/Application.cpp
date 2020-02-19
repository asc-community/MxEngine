// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
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

#include "Application.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Math/Math.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Core/Event/RenderEvent.h"
#include "Core/Event/UpdateEvent.h"
#include "Core/Event/FpsUpdateEvent.h"
#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Camera/OrthographicCamera.h"
#include "Library/Scripting/ScriptEngine.h"

#include <fstream>

#include <Platform/OpenGL/GraphicFactory/GLGraphicFactory.h>

namespace MxEngine
{
    void Application::CreateConsoleBindings(DeveloperConsole& console)
    {
        console.SetSize({ this->GetWindow().GetWidth() / 2.0f, this->GetWindow().GetHeight() / 2.0f });

        this->GetEventDispatcher().AddEventListener<RenderEvent>("DeveloperConsole",
            [this](RenderEvent&) { this->Console.OnRender(); });

        auto& script = console.GetEngine();

        // Vector3
        script.AddType<Vector3>("vec3");
        script.AddTypeConstructor<Vector3, float>("vec3");
        script.AddTypeConstructor<Vector3, float, float, float>("vec3");
        script.AddReference("x", &Vector3::x);
        script.AddReference("y", &Vector3::y);
        script.AddReference("z", &Vector3::z);

        // CameraController
        using TranslateFunc = CameraController & (CameraController::*)(float, float, float);
        script.AddReference("position", &CameraController::GetPosition);
        script.AddReference("direction", &CameraController::GetDirection);
        script.AddReference("up", &CameraController::GetUpVector);
        script.AddReference("zoom", &CameraController::GetZoom);
        script.AddReference("set_zoom", &CameraController::SetZoom);
        script.AddReference("rotate", &CameraController::Rotate);
        script.AddReference("translate", (TranslateFunc)& CameraController::Translate);
        script.AddReference("translate_x", &CameraController::TranslateX);
        script.AddReference("translate_y", &CameraController::TranslateY);
        script.AddReference("translate_z", &CameraController::TranslateZ);
        script.AddReference("move_forward", &CameraController::TranslateForward);
        script.AddReference("move_right", &CameraController::TranslateRight);
        script.AddReference("move_up", &CameraController::TranslateUp);

        // Camera
        script.AddReference("set_zfar",
            [](float zfar) { Context::Instance()->GetRenderer().ViewPort.GetCamera().SetZFar(zfar); });
        script.AddReference("set_znear", 
            [](float znear) { Context::Instance()->GetRenderer().ViewPort.GetCamera().SetZNear(znear); });
        script.AddReference("zfar",
            []() { return Context::Instance()->GetRenderer().ViewPort.GetCamera().GetZFar(); });
        script.AddReference("znear",
            []() { return Context::Instance()->GetRenderer().ViewPort.GetCamera().GetZNear(); });

        // Object
        using ScaleFunc1F = Object & (Object::*)(float);
        using ScaleFunc3F = Object & (Object::*)(float, float, float);
        using TranslateFunc3 = Object & (Object::*)(float, float, float);
        using RotateMoveFunc = Object & (Object::*)(float);
        using RotateFunc2F = Object & (Object::*)(float, float);
        script.AddReference("rotate", (RotateFunc2F)&Object::Rotate);
        script.AddReference("rotate_x", &Object::RotateX);
        script.AddReference("rotate_y", &Object::RotateY);
        script.AddReference("rotate_z", &Object::RotateZ);
        script.AddReference("scale", (ScaleFunc1F)&Object::Scale);
        script.AddReference("scale", (ScaleFunc3F)&Object::Scale);
        script.AddReference("translate", (TranslateFunc3)&Object::Translate);
        script.AddReference("translate_x", &Object::TranslateX);
        script.AddReference("translate_y", &Object::TranslateY);
        script.AddReference("translate_z", &Object::TranslateZ);
        script.AddReference("move_forward", (RotateMoveFunc)&Object::TranslateForward);
        script.AddReference("move_right", (RotateMoveFunc)&Object::TranslateRight);
        script.AddReference("move_up", (RotateMoveFunc)&Object::TranslateUp);
        script.AddReference("forward_vec", &Object::GetForwardVector);
        script.AddReference("up_vec", &Object::GetUpVector);
        script.AddReference("right_vec", &Object::GetRightVector);
        script.AddReference("set_forward", &Object::SetForwardVector);
        script.AddReference("set_up", &Object::SetUpVector);
        script.AddReference("set_right", &Object::SetRightVector);
        script.AddReference("hide", &Object::Hide);
        script.AddReference("show", &Object::Show);                  
        script.AddReference("translation", &Object::GetTranslation);
        script.AddReference("rotation", &Object::GetRotation);
        script.AddReference("scale", &Object::GetScale);
           
        // Application
        script.AddVariable("objects", Context::Instance()->objects);
        script.AddVariable("viewport", Context::Instance()->GetRenderer().ViewPort);
        script.AddReference("load", Context::Instance(), &Application::CreateObject);
        script.AddReference("delete", Context::Instance(), &Application::DestroyObject);
        script.AddReference("exit", Context::Instance(), &Application::CloseApplication);
        script.AddReference("dt", Context::Instance(), &Application::GetTimeDelta);
        script.AddReference("[]",
            [](Application::ObjectStorage& storage, const std::string& name) -> Object& 
            { 
                return *storage[name]; 
            });
        script.AddReference("to_string",
            [](const Vector3& vec)
            {
                return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
            });
        script.AddReference("set_texture",
            [](Object& instance, const std::string& path)
            {
                instance.Texture = Context::Instance()->CreateTexture(path);
            });
        script.AddReference("set_shader",
            [](Object& instance, const std::string& vertex, const std::string& fragment)
            {
                instance.Shader = Context::Instance()->CreateShader(vertex, fragment);
            });
        script.AddReference("remove_event",
            [](const std::string& name)
            {
                Context::Instance()->GetEventDispatcher().RemoveEventListener(name);
            });
        script.AddReference("orthographic",
            [](CameraController& viewport)
            {
                viewport.SetCamera(MakeUnique<OrthographicCamera>());
            });
        script.AddReference("perspective",
            [](CameraController& viewport)
            {
                viewport.SetCamera(MakeUnique<PerspectiveCamera>());
            }
        );
        script.AddReference("on_update",
            [](const std::string& eventName, std::function<void()> func)
            {
                Context::Instance()->GetEventDispatcher().AddEventListener<UpdateEvent>(eventName,
                    [name = eventName, func = std::move(func)](UpdateEvent&) 
                {
                    try 
                    { 
                        func(); 
                    }
                    catch (std::exception& e) 
                    { 
                        std::string error = e.what();
                        auto it = error.find("Error:");
                        if (it != error.npos)
                        {
                            error.erase(it, it + 6);
                            error = "[error]:" + error;
                        }
                        Context::Instance()->Console.Log(error); 
                        Context::Instance()->GetEventDispatcher().RemoveEventListener(name);
                    }
                });
            });
    }

    Application::Application()
        : manager(this), window(Graphics::Instance()->CreateWindow(1280, 720, "MxEngine Application")),
        TimeDelta(0), CounterFPS(0), renderer(Graphics::Instance()->GetRenderer())
    {
        this->GetWindow().UseEventDispatcher(&this->Dispatcher);
	}

    void Application::ToggleMeshDrawing(bool state)
    {
        this->debugMeshDraw = state;
    }

	Window& Application::GetWindow()
	{
		return *this->window;
	}

	float Application::GetTimeDelta() const
	{
		return this->TimeDelta;
	}

	RenderController& Application::GetRenderer()
	{
        return this->renderer;
	}

    Ref<RenderObjectContainer> Application::LoadObjectBase(const std::string& filepath)
    {
        return MakeRef<RenderObjectContainer>(this->ResourcePath + filepath);
    }

    Object& Application::CreateObject(const std::string& name, const std::string& path)
	{
        MAKE_SCOPE_PROFILER("Application::CreateObject");
		if (this->objects.find(name) != this->objects.end())
		{
			Logger::Instance().Warning("MxEngine::Application", "overriding already existing object: " + name);
			DestroyObject(name);
		}
		this->objects.emplace(name, MakeUnique<Object>(this->LoadObjectBase(path)));
        return *this->objects[name];
	}

    Object& Application::AddObject(const std::string& name, UniqueRef<Object> object)
    {
        if (this->objects.find(name) != this->objects.end())
        {
            Logger::Instance().Warning("MxEngine::Application", "overriding already existing object: " + name);
            DestroyObject(name);
        }
        this->objects.emplace(name, std::move(object));
        return *this->objects[name];
    }

    Ref<Texture> Application::CreateTexture(const std::string& filepath, bool genMipmaps, bool flipImage)
    {
        MAKE_SCOPE_PROFILER("Application::CreateTexture");
        auto texture = Graphics::Instance()->CreateTexture(this->ResourcePath + filepath, genMipmaps, flipImage);
        return Ref<Texture>(texture.release());
    }

    Ref<Shader> Application::CreateShader(const std::string& vertexShaderPath, const std::string fragmentShaderPath)
    {
        MAKE_SCOPE_PROFILER("Application::CreateShader");
        auto shader = Graphics::Instance()->CreateShader(this->ResourcePath + vertexShaderPath, this->ResourcePath + fragmentShaderPath);
        return Ref<Shader>(shader.release());
    }

	Object& Application::GetObject(const std::string& name)
	{
		if (this->objects.find(name) == this->objects.end())
		{
			Logger::Instance().Warning("MxEngine::Application", "object was not found: " + name);
			return this->defaultInstance;
		}
		return *this->objects[name];
	}

	void Application::DestroyObject(const std::string& name)
	{
		if (this->objects.find(name) == this->objects.end())
		{
			Logger::Instance().Warning("MxEngine::Application", "trying to destroy object which not exists: " + name);
			return;
		}
		this->objects.erase(name);
	}

    void Application::ToggleDeveloperConsole(bool isVisible)
    {
        this->Console.Toggle(isVisible);
        this->GetWindow().UseEventDispatcher(isVisible ? nullptr : &this->Dispatcher);
    }

	void Application::DrawObjects(bool meshes) const
	{
        MAKE_SCOPE_PROFILER("Application::DrawObjects");
		if (meshes)
		{
			for (const auto& object : this->objects)
			{
				this->renderer.DrawObject(*object.second);
				this->renderer.DrawObjectMesh(*object.second);
			}
		}
		else
		{
			for (const auto& object : this->objects)
			{
				this->renderer.DrawObject(*object.second);
			}
		}
	}

    void Application::InvokeUpdate()
    {
        this->GetWindow().OnUpdate();
        MAKE_SCOPE_PROFILER("MxEngine::OnUpdate");
        UpdateEvent updateEvent(this->TimeDelta);
        this->GetEventDispatcher().Invoke(updateEvent);
        for (const auto& object : this->objects)
        {
            object.second->OnUpdate();
        }
        this->OnUpdate();
    }

	void Application::CloseApplication()
	{
        this->shouldClose = true;
	}

	void Application::CreateContext()
	{
        MAKE_SCOPE_PROFILER("Application::CreateContext");
		this->GetWindow()
			.UseProfile(3, 3, Profile::CORE)
			.UseCursorMode(CursorMode::DISABLED)
			.UseSampling(4)
			.UseDoubleBuffering(false)
			.UseTitle("MxEngine Project")
			.UsePosition(600, 300)
			.Create();

        auto& renderingEngine = this->renderer.GetRenderEngine();
        renderingEngine
            .UseDepthBuffer()
            .UseCulling()
            .UseSampling()
            .UseClearColor(0.0f, 0.0f, 0.0f)
            .UseTextureMagFilter(MagFilter::NEAREST)
            .UseTextureMinFilter(MinFilter::LINEAR_MIPMAP_LINEAR)
            .UseTextureWrap(WrapType::REPEAT, WrapType::REPEAT)
            .UseBlending(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA)
            .UseAnisotropicFiltering(renderingEngine.GetLargestAnisotropicFactor());

        this->CreateConsoleBindings(this->Console);
	}

	AppEventDispatcher& Application::GetEventDispatcher()
	{
		return this->Dispatcher;
	}

	void Application::Run()
	{
        if (!this->GetWindow().IsCreated())
        {
            Logger::Instance().Debug("MxEngine::Application", "window was not created, aborting...");
            return;
        }

        if (this->Console.IsToggled())
        {
            this->Console.Log("Welcome to MxEngine developer console!");
            this->Console.Log("This console is powered by ChaiScript: http://chaiscript.com");
        }

        {
            MAKE_SCOPE_PROFILER("Application::OnCreate");
            MAKE_SCOPE_TIMER("MxEngine::Application", "Application::OnCreate()");
            this->OnCreate();
        }

		float secondEnd = Time::Current(), frameEnd = Time::Current();
		int fpsCounter = 0;

        {
            MAKE_SCOPE_PROFILER("Application::Run");
            Logger::Instance().Debug("MxEngine::Application", "starting main loop...");
            while (this->GetWindow().IsOpen())
            {
                fpsCounter++;
                float now = Time::Current();
                if (now - secondEnd >= 1.0f)
                {
                    this->CounterFPS = fpsCounter;
                    fpsCounter = 0;
                    secondEnd = now;
                    this->Dispatcher.AddEvent(MakeUnique<FpsUpdateEvent>(this->CounterFPS));
                }
                TimeDelta = now - frameEnd;
                frameEnd = now;

                // event phase
                {
                    MAKE_SCOPE_PROFILER("Application::ProcessEvents");
                    this->Dispatcher.InvokeAll();
                    if (this->shouldClose) break;
                }

                InvokeUpdate();
                
                this->GetRenderer().Clear();
                this->DrawObjects(this->debugMeshDraw);

                static RenderEvent renderEvent;
                this->GetEventDispatcher().Invoke(renderEvent);
                this->renderer.Render();
                this->GetWindow().PullEvents(); 
                if (this->shouldClose) break;
            }

            // application exit
            {
                MAKE_SCOPE_PROFILER("Application::CloseApplication");
                this->OnDestroy();
                this->GetWindow().Close();
            }
        }
	}

	Application::~Application()
	{
        this->GetRenderer().DefaultTexture.reset();
        this->GetRenderer().MeshShader.reset();
        this->GetRenderer().ObjectShader.reset();

        Logger::Instance().Debug("MxEngine::Application", "application destroyed");
	}

    Application::ModuleManager::ModuleManager(Application* app)
    {
        Profiler::Instance().StartSession("profile_log.json");
        
        assert(Context::Instance() == nullptr);
        Context::Instance() = app;

        RenderEngine renderer = RenderEngine::OpenGL;
        switch (renderer)
        {
        case RenderEngine::OpenGL:
            Graphics::Instance().reset(Alloc<GLGraphicFactory>());
            break;
        default:
            Logger::Instance().Error("MxEngine::Application", "No Rendering Engine was provided");
            return;
        }
        Graphics::Instance()->GetGraphicModule().Init();
    }

    Application::ModuleManager::~ModuleManager()
    {
        Graphics::Instance()->GetGraphicModule().Destroy();
        Profiler::Instance().EndSession();
    }
}