#include "Application.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Math/Math.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Core/Event/RenderEvent.h"
#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Camera/OrthographicCamera.h"
#include "Library/Scripting/ScriptEngine.h"

#include <fstream>

#include <Platform/OpenGL/GraphicFactory/GLGraphicFactory.h>

namespace MxEngine
{
    void Application::CreateConsoleBindings(DeveloperConsole& console)
    {
        this->Console.SetSize({ this->GetWindow().GetWidth() / 3.0f, this->GetWindow().GetHeight() / 2.0f });

        this->GetEventDispatcher().AddEventListener<RenderEvent>("DeveloperConsole", 
            [this] (RenderEvent&) { this->Console.OnRender(); });

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

        // ObjectInstance
        using ScaleFunc1F = ObjectInstance & (ObjectInstance::*)(float);
        using ScaleFunc3F = ObjectInstance & (ObjectInstance::*)(float, float, float);
        using TranslateFunc3 = ObjectInstance & (ObjectInstance::*)(float, float, float);
        using RotateMoveFunc = ObjectInstance & (ObjectInstance::*)(float);
        using RotateFunc2F = ObjectInstance & (ObjectInstance::*)(float, float);
        script.AddReference("rotate", (RotateFunc2F)&ObjectInstance::Rotate);
        script.AddReference("rotate_x", &ObjectInstance::RotateX);
        script.AddReference("rotate_y", &ObjectInstance::RotateY);
        script.AddReference("rotate_z", &ObjectInstance::RotateZ);
        script.AddReference("scale", (ScaleFunc1F)&ObjectInstance::Scale);
        script.AddReference("scale", (ScaleFunc3F)&ObjectInstance::Scale);
        script.AddReference("translate", (TranslateFunc3)&ObjectInstance::Translate);
        script.AddReference("translate_x", &ObjectInstance::TranslateX);
        script.AddReference("translate_y", &ObjectInstance::TranslateY);
        script.AddReference("translate_z", &ObjectInstance::TranslateZ);
        script.AddReference("move_forward", (RotateMoveFunc)&ObjectInstance::TranslateForward);
        script.AddReference("move_right", (RotateMoveFunc)&ObjectInstance::TranslateRight);
        script.AddReference("move_up", (RotateMoveFunc)&ObjectInstance::TranslateUp);
        script.AddReference("forward_vec", &ObjectInstance::GetForwardVector);
        script.AddReference("up_vec", &ObjectInstance::GetUpVector);
        script.AddReference("right_vec", &ObjectInstance::GetRightVector);
        script.AddReference("set_forward", &ObjectInstance::SetForwardVector);
        script.AddReference("set_up", &ObjectInstance::SetUpVector);
        script.AddReference("set_right", &ObjectInstance::SetRightVector);
        script.AddReference("hide", &ObjectInstance::Hide);
        script.AddReference("show", &ObjectInstance::Show);                  
        script.AddReference("translation", &ObjectInstance::GetTranslation);
        script.AddReference("rotation", &ObjectInstance::GetRotation);
        script.AddReference("scale", &ObjectInstance::GetScale);
           
        // Application
        script.AddVariable("objects", Context::Instance()->objects);
        script.AddVariable("viewport", Context::Instance()->GetRenderer().ViewPort);
        script.AddReference("load", Context::Instance(), &Application::CreateObject);
        script.AddReference("delete", Context::Instance(), &Application::DestroyObject);
        script.AddReference("exit", Context::Instance(), &Application::CloseApplication);
        script.AddReference("[]",
            [](Application::ObjectStorage& storage, const std::string& name) -> ObjectInstance& 
            { 
                return storage[name]; 
            });
        script.AddReference("to_string",
            [](const Vector3& vec)
            {
                return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
            });
        script.AddReference("set_texture",
            [](ObjectInstance& instance, const std::string& path)
            {
                instance.Texture = Context::Instance()->CreateTexture(path);
            });
        script.AddReference("set_shader",
            [](ObjectInstance& instance, const std::string& vertex, const std::string& fragment)
            {
                instance.Shader = Context::Instance()->CreateShader(vertex, fragment);
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
    }

    Application::Application()
        : manager(this), window(Graphics::Instance()->CreateWindow(1280, 720, "MxEngine Application")),
        TimeDelta(0), CounterFPS(0), renderer(Graphics::Instance()->GetRenderer())
    {
        this->GetWindow().UseEventDispatcher(&this->Dispatcher);
        this->CreateConsoleBindings(this->Console);
	}

	Window& Application::GetWindow()
	{
		return *this->window;
	}

	TimeStep Application::GetTimeDelta() const
	{
		return this->TimeDelta;
	}

	void Application::SetWindow(UniqueRef<Window> window)
	{
		this->window = std::move(window);
		this->window->UseEventDispatcher(&this->Dispatcher);
	}

	RenderController& Application::GetRenderer()
	{
        return this->renderer;
	}

	ObjectInstance& Application::CreateObject(const std::string& name, const std::string& path)
	{
        MAKE_SCOPE_PROFILER("Application::CreateObject");
		if (this->objects.find(name) != this->objects.end())
		{
			Logger::Instance().Warning("MxEngine::Application", "overriding already existing object: " + name);
			DestroyObject(name);
		}
		this->objects.insert({ name, ObjectInstance(MakeRef<BaseObject>(this->ResourcePath + path)) });
		return GetObject(name);
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

	ObjectInstance& Application::GetObject(const std::string& name)
	{
		if (this->objects.find(name) == this->objects.end())
		{
			Logger::Instance().Warning("MxEngine::Application", "object was not found: " + name);
			return this->defaultInstance;
		}
		return this->objects[name];
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
    }

	void Application::DrawObjects(bool meshes) const
	{
        MAKE_SCOPE_PROFILER("Application::DrawObjects");
		if (meshes)
		{
			for (const auto& object : this->objects)
			{
				this->renderer.DrawObject(object.second);
				this->renderer.DrawObjectMesh(object.second);
			}
		}
		else
		{
			for (const auto& object : this->objects)
			{
				this->renderer.DrawObject(object.second);
			}
		}
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
	}

	AppEventDispatcher& Application::GetEventDispatcher()
	{
		return this->Dispatcher;
	}

	void Application::Run()
	{
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
                }
                TimeDelta = now - frameEnd;
                frameEnd = now;

                // event phase
                {
                    MAKE_SCOPE_PROFILER("Application::ProcessEvents");
                    this->Dispatcher.InvokeAll();
                    if (this->shouldClose) break;
                }

                this->GetWindow().OnUpdate();
                {
                    MAKE_SCOPE_PROFILER("Application::OnUpdate");
                    this->OnUpdate();
                }
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