#include "Application.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Math/Math.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Utilities/Profiler/Profiler.h"
#include <fstream>

#include <Platform/OpenGL/GraphicFactory/GLGraphicFactory.h>

namespace MomoEngine
{
    void Application::CreateConsoleBindings(DeveloperConsole& console)
    {
        // CameraController
        using TranslateFunc = CameraController & (CameraController::*)(float, float, float);
        console.AddReference("position", &CameraController::GetPosition);
        console.AddReference("direction", &CameraController::GetDirection);
        console.AddReference("up", &CameraController::GetUpVector);
        console.AddReference("zoom", &CameraController::GetZoom);
        console.AddReference("set_zoom", &CameraController::SetZoom);
        console.AddReference("rotate", &CameraController::Rotate);
        console.AddReference("translate", (TranslateFunc)& CameraController::Translate);
        console.AddReference("translate_x", &CameraController::TranslateX);
        console.AddReference("translate_y", &CameraController::TranslateY);
        console.AddReference("translate_z", &CameraController::TranslateZ);
        console.AddReference("move_forward", &CameraController::TranslateForward);
        console.AddReference("move_right", &CameraController::TranslateRight);
        console.AddReference("move_up", &CameraController::TranslateUp);

        // Camera
        console.AddReference("set_zfar",
            [this](float zfar) { this->GetRenderer().ViewPort.GetCamera().SetZFar(zfar); });
        console.AddReference("set_znear", 
            [this](float znear) { this->GetRenderer().ViewPort.GetCamera().SetZNear(znear); });
        console.AddReference("zfar",
            [this]() { return this->GetRenderer().ViewPort.GetCamera().GetZFar(); });
        console.AddReference("znear",
            [this]() { return this->GetRenderer().ViewPort.GetCamera().GetZNear(); });

        // ObjectInstance
        using ScaleFunc1F = ObjectInstance & (ObjectInstance::*)(float);
        using ScaleFunc3F = ObjectInstance & (ObjectInstance::*)(float, float, float);
        console.AddReference("rotate_x", &ObjectInstance::RotateX);
        console.AddReference("rotate_y", &ObjectInstance::RotateY);
        console.AddReference("rotate_z", &ObjectInstance::RotateZ);
        console.AddReference("scale", (ScaleFunc1F)&ObjectInstance::Scale);
        console.AddReference("scale", (ScaleFunc3F)&ObjectInstance::Scale);
        console.AddReference("translate", &ObjectInstance::Translate);
        console.AddReference("translate_x", &ObjectInstance::TranslateX);
        console.AddReference("translate_y", &ObjectInstance::TranslateY);
        console.AddReference("translate_z", &ObjectInstance::TranslateZ);
        console.AddReference("hide", &ObjectInstance::Hide);
        console.AddReference("show", &ObjectInstance::Show);                  
        console.AddReference("translation", &ObjectInstance::GetTranslation);
        console.AddReference("rotation", &ObjectInstance::GetRotation);
        console.AddReference("scale", &ObjectInstance::GetScale);
           
        // Application
        console.AddVariable("objects", this->objects);
        console.AddVariable("viewport", this->GetRenderer().ViewPort);
        console.AddReference("load", this, &Application::CreateObject);
        console.AddReference("delete", this, &Application::DestroyObject);
        console.AddReference("[]",
            [](Application::ObjectStorage& storage, const std::string& name) -> ObjectInstance& 
            { 
                return storage[name]; 
            });
        console.AddReference("to_string",
            [](const Vector3& vec)
            {
                return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
            });
        console.AddReference("set_texture",
            [this](ObjectInstance& instance, const std::string& path)
            {
                instance.Texture = this->CreateTexture(path);
            });
        console.AddReference("set_shader",
            [this](ObjectInstance& instance, const std::string& vertex, const std::string& fragment)
            {
                instance.Shader = this->CreateShader(vertex, fragment);
            });
    }

    Application::Application()
		: window(Graphics::Instance()->CreateWindow(1280, 720, "MomoEngine Application")), 
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
			Logger::Instance().Warning("MomoEngine::Application", "overriding already existing object: " + name);
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
			Logger::Instance().Warning("MomoEngine::Application", "object was not found: " + name);
			return this->defaultInstance;
		}
		return this->objects[name];
	}

	void Application::DestroyObject(const std::string& name)
	{
		if (this->objects.find(name) == this->objects.end())
		{
			Logger::Instance().Warning("MomoEngine::Application", "trying to destroy object which not exists: " + name);
			return;
		}
		this->objects.erase(name);
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
        MAKE_SCOPE_PROFILER("Application::CloseApplication");
		this->OnDestroy();
		this->GetWindow().Close();
	}

	void Application::CreateContext()
	{
        MAKE_SCOPE_PROFILER("Application::CreateContext");
		this->GetWindow()
			.UseProfile(3, 3, Profile::CORE)
			.UseCursorMode(CursorMode::DISABLED)
			.UseSampling(4)
			.UseDoubleBuffering(false)
			.UseTitle("MomoEngine Project")
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
            MAKE_SCOPE_TIMER("MomoEngine::Application", "Application::OnCreate()");
            this->OnCreate();
        }

		float secondEnd = Time::Current(), frameEnd = Time::Current();
		int fpsCounter = 0;

        {
            MAKE_SCOPE_PROFILER("Application::Run");
            Logger::Instance().Debug("MomoEngine::Application", "starting main loop...");
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
                    if (!this->GetWindow().IsOpen()) break;
                }

                this->GetWindow().OnUpdate();
                {
                    MAKE_SCOPE_PROFILER("Application::OnUpdate");
                    float onUpdateStart = Time::Current();
                    this->OnUpdate();
                    if (!this->GetWindow().IsOpen()) break;
                    float onUpdateEnd = Time::Current();
                    if (onUpdateEnd - onUpdateStart > 0.01666f)
                    {
                        if (onUpdateEnd - onUpdateStart > 0.03333f)
                            Logger::Instance().Error("MomoEngine::Application", "Application::OnUpdate running more than 33.33ms");
                        else
                            Logger::Instance().Warning("MomoEngine::Application", "Application::OnUpdate running more than 16.66ms");
                    }
                }
                this->renderer.Render();
                this->GetWindow().PullEvents();
            }
        }
	}

	Application::~Application()
	{
        this->GetRenderer().DefaultTexture.reset();
        this->GetRenderer().MeshShader.reset();
        this->GetRenderer().ObjectShader.reset();

        Logger::Instance().Debug("MomoEngine::Application", "application destroyed");
	}

    Application::ModuleManager::ModuleManager()
    {
        Profiler::Instance().StartSession("profile_log.json");

        RenderEngine renderer = RenderEngine::OpenGL;
        switch (renderer)
        {
        case RenderEngine::OpenGL:
            Graphics::Instance().reset(Alloc<GLGraphicFactory>());
            break;
        default:
            Logger::Instance().Error("MomoEngine::Application", "No Rendering Engine was provided");
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