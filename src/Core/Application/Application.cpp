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

#include "Application.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Math/Math.h"
#include "Utilities/Profiler/Profiler.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"
#include "Core/Event/RenderEvent.h"
#include "Core/Event/UpdateEvent.h"
#include "Core/Event/FpsUpdateEvent.h"
#include "Core/Event/MouseEvent.h"
#include "Core/Event/AppDestroyEvent.h"
#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Camera/OrthographicCamera.h"

#include <fstream>

// conditional includes
#include "Core/Macro/Macro.h"

#include "Platform/OpenGL/GraphicFactory/GLGraphicFactory.h"
#include "Library/Scripting/ChaiScript/ChaiScriptEngine.h"
#include "Library/Scripting/Python/PythonEngine.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
	Application::Application()
		: manager(this), window(Graphics::Instance()->CreateWindow(1280, 720, "MxEngine Application")),
		TimeDelta(0), CounterFPS(0), renderer(Graphics::Instance()->GetRenderer())
	{
		this->GetWindow().UseEventDispatcher(&this->dispatcher);
	}

	void Application::ToggleMeshDrawing(bool state)
	{
		this->debugMeshDraw = state;
	}

	void Application::OnCreate()
	{
		// must be overwritten in derived class
	}

	void Application::OnUpdate()
	{
		// must be overwritten in derived class
	}

	void Application::OnDestroy()
	{
		// is overriden in derived class
	}

	Window& Application::GetWindow()
	{
		return *this->window;
	}

    Counter::CounterType Application::GenerateResourceId()
    {
		return this->resourceIdCounter++;
    }

    float Application::GetTimeDelta() const
	{
		return this->TimeDelta;
	}

	RenderController& Application::GetRenderer()
	{
		return this->renderer;
	}

	LoggerImpl& Application::GetLogger()
	{
		return Logger::Instance();
	}

	Mesh* Application::LoadMesh(const FilePath& filepath)
	{
		auto mesh = MakeUnique<Mesh>((this->ResourcePath / filepath).string());
		Mesh* ret = mesh.get();
		auto name = Format(FMT_STRING("Mesh_{0}"), this->GenerateResourceId());
		this->GetResourceManager<Mesh>().Add(name, std::move(mesh));
		return ret;
	}

	MxObject& Application::CreateObject(const std::string& name, const FilePath& filepath)
	{
		MAKE_SCOPE_PROFILER("Application::CreateObject");
		if (this->objects.Get().find(name) != this->objects.Get().end())
		{
			Logger::Instance().Warning("MxEngine::Application", "overriding already existing object: " + name);
			DestroyObject(name);
		}
		auto ptr = MakeUnique<MxObject>(this->LoadMesh(filepath));
		auto& object = *ptr;
		this->objects.Add(name, std::move(ptr));
		return object;
	}

	MxObject& Application::AddObject(const std::string& name, UniqueRef<MxObject> object)
	{
		auto& value = *object;
		this->objects.Add(name, std::move(object));
		return value;
	}

	MxObject& Application::CopyObject(const std::string& name, const std::string& existingObject)
	{
		auto& object = GetObject(existingObject);
		return AddObject(name, MakeUnique<MxObject>(&object.GetMesh()));
	}

	const Application::ObjectStorage::Storage& Application::GetObjectList()
	{
		return this->objects.Get();
	}

	void Application::InvalidateObjects()
	{
		this->objects.Update();
	}

	Texture* Application::CreateTexture(const FilePath& texture, bool genMipmaps, bool flipImage)
	{
		MAKE_SCOPE_PROFILER("Application::CreateTexture");
		auto textureObject = Graphics::Instance()->CreateTexture(this->ResourcePath / texture, genMipmaps, flipImage);
		auto name = Format(FMT_STRING("MxTexture_{0}"), this->GenerateResourceId());
		return this->GetResourceManager<Texture>().Add(name, std::move(textureObject));
	}

	Shader* Application::CreateShader(const FilePath& vertex, const FilePath& fragment)
	{
		MAKE_SCOPE_PROFILER("Application::CreateShader");
		auto shader = Graphics::Instance()->CreateShader(this->ResourcePath / vertex, this->ResourcePath / fragment);
		auto name = Format(FMT_STRING("MxShader_{0}"), this->GenerateResourceId());
		return this->GetResourceManager<Shader>().Add(name, std::move(shader));
	}

	void Application::ExecuteScript(const FilePath& script)
	{
		std::ifstream file(this->ResourcePath / script);
	}

	MxObject& Application::GetObject(const std::string& name)
	{
		if (this->objects.Get().find(name) == this->objects.Get().end())
		{
			Logger::Instance().Warning("MxEngine::Application", "object was not found: " + name);
			return this->defaultInstance;
		}
		return *this->objects.Get()[name];
	}

	void Application::DestroyObject(const std::string& name)
	{
		if (this->objects.Get().find(name) == this->objects.Get().end())
		{
			Logger::Instance().Warning("MxEngine::Application", "trying to destroy object which not exists: " + name);
			return;
		}
		this->objects.Remove(name);
	}

	void Application::ToggleDeveloperConsole(bool isVisible)
	{
		this->GetConsole().Toggle(isVisible);
		this->GetWindow().UseEventDispatcher(isVisible ? nullptr : &this->dispatcher);
	}

	void Application::DrawObjects(bool meshes) const
	{
		MAKE_SCOPE_PROFILER("Application::DrawObjects");
		if (meshes)
		{
			for (const auto& object : this->objects.Get())
			{
				this->renderer.DrawObject(*object.second);
				this->renderer.DrawObjectMesh(*object.second);
			}
		}
		else
		{
			for (const auto& object : this->objects.Get())
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
		for (const auto& object : this->objects.Get())
		{
			object.second->OnUpdate();
		}
		this->objects.Update();
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

		this->CreateConsoleBindings(this->GetConsole());
	}

	AppEventDispatcher& Application::GetEventDispatcher()
	{
		return this->dispatcher;
	}

	DeveloperConsole& Application::GetConsole()
	{
		return this->console;
	}

	void Application::Run()
	{
		if (!this->GetWindow().IsCreated())
		{
			Logger::Instance().Error("MxEngine::Application", "window was not created, aborting...");
			return;
		}

		if (this->GetConsole().IsToggled())
		{
			this->GetConsole().Log("Welcome to MxEngine developer console!");
			#if defined(MXENGINE_USE_CHAISCRIPT)
			this->GetConsole().Log("This console is powered by ChaiScript: http://chaiscript.com");
			#elif defined(MXENGINE_USE_PYTHON)
			this->GetConsole().Log("This console is powered by Python: https://www.python.org");
			#endif
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
					this->GetEventDispatcher().AddEvent(MakeUnique<FpsUpdateEvent>(this->CounterFPS));
				}
				TimeDelta = now - frameEnd;
				frameEnd = now;

				// event phase
				{
					MAKE_SCOPE_PROFILER("Application::ProcessEvents");
					this->GetEventDispatcher().InvokeAll();
					if (this->shouldClose) break;
				}

				InvokeUpdate();
				
				this->GetRenderer().Clear();
				this->DrawObjects(this->debugMeshDraw);

				RenderEvent renderEvent;
				this->GetEventDispatcher().Invoke(renderEvent);
				this->renderer.Render();
				this->GetWindow().PullEvents(); 
				if (this->shouldClose) break;
			}

			// application exit
			{
				MAKE_SCOPE_PROFILER("Application::CloseApplication");
				AppDestroyEvent appDestroyEvent;
				this->GetEventDispatcher().Invoke(appDestroyEvent);
				this->OnDestroy();
				this->GetWindow().Close();
			}
		}
	}

	Application::~Application()
	{
		{
			MAKE_SCOPE_PROFILER("Application::DestroyObjects");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::DestroyObjects");
			this->objects.Get().clear();
		}
		Logger::Instance().Debug("MxEngine::Application", "application destroyed");
	}

	Application* Application::Get()
	{
		return Application::Current;
	}

	void Application::Set(Application* application)
	{
		Application::Current = application;
	}

	Application::ModuleManager::ModuleManager(Application* app)
	{
		Profiler::Instance().StartSession("profile_log.json");
		
		assert(Application::Get() == nullptr);
		Application::Set(app);

#if defined(MXENGINE_USE_OPENGL)
		Graphics::Instance() = Alloc<GLGraphicFactory>();
#else
		Graphics::Instance() = nullptr;
		Logger::Instance().Error("MxEngine::Application", "No Rendering Engine was provided");
		return;
#endif
		Graphics::Instance()->GetGraphicModule().Init();
	}

	Application::ModuleManager::~ModuleManager()
	{
		Graphics::Instance()->GetGraphicModule().Destroy();
		Profiler::Instance().EndSession();
	}

	#if defined(MXENGINE_USE_CHAISCRIPT)
	void Application::CreateConsoleBindings(DeveloperConsole& console)
	{
		console.SetSize({ this->GetWindow().GetWidth() / 2.5f, this->GetWindow().GetHeight() / 2.0f });
		this->GetEventDispatcher().AddEventListener<RenderEvent>("DeveloperConsole",
			[this](RenderEvent&) { this->GetConsole().OnRender(); });

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
		script.AddReference("translate", (TranslateFunc)&CameraController::Translate);
		script.AddReference("translate_x", &CameraController::TranslateX);
		script.AddReference("translate_y", &CameraController::TranslateY);
		script.AddRseference("translate_z", &CameraController::TranslateZ);
		script.AddReference("move_forward", &CameraController::TranslateForward);
		script.AddReference("move_right", &CameraController::TranslateRight);
		script.AddReference("move_up", &CameraController::TranslateUp);

		// Camera
		script.AddReference("set_zfar",
			[](float zfar) { Application::Get()->GetRenderer().ViewPort.GetCamera().SetZFar(zfar); });
		script.AddReference("set_znear",
			[](float znear) { Application::Get()->GetRenderer().ViewPort.GetCamera().SetZNear(znear); });
		script.AddReference("zfar",
			[]() { return Application::Get()->GetRenderer().ViewPort.GetCamera().GetZFar(); });
		script.AddReference("znear",
			[]() { return Application::Get()->GetRenderer().ViewPort.GetCamera().GetZNear(); });

		// Object
		using ScaleFunc1F = MxObject & (MxObject::*)(float);
		using ScaleFunc3F = MxObject & (MxObject::*)(float, float, float);
		using TranslateFunc3 = MxObject & (MxObject::*)(float, float, float);
		using RotateMoveFunc = MxObject & (MxObject::*)(float);
		using RotateFunc2F = MxObject & (MxObject::*)(float, float);
		script.AddReference("rotate", (RotateFunc2F)&MxObject::Rotate);
		script.AddReference("rotate_x", &MxObject::RotateX);
		script.AddReference("rotate_y", &MxObject::RotateY);
		script.AddReference("rotate_z", &MxObject::RotateZ);
		script.AddReference("scale", (ScaleFunc1F)&MxObject::Scale);
		script.AddReference("scale", (ScaleFunc3F)&MxObject::Scale);
		script.AddReference("translate", (TranslateFunc3)&MxObject::Translate);
		script.AddReference("translate_x", &MxObject::TranslateX);
		script.AddReference("translate_y", &MxObject::TranslateY);
		script.AddReference("translate_z", &MxObject::TranslateZ);
		script.AddReference("move_forward", (RotateMoveFunc)&MxObject::TranslateForward);
		script.AddReference("move_right", (RotateMoveFunc)&MxObject::TranslateRight);
		script.AddReference("move_up", (RotateMoveFunc)&MxObject::TranslateUp);
		script.AddReference("forward_vec", &MxObject::GetForwardVector);
		script.AddReference("up_vec", &MxObject::GetUpVector);
		script.AddReference("right_vec", &MxObject::GetRightVector);
		script.AddReference("set_forward", &MxObject::SetForwardVector);
		script.AddReference("set_up", &MxObject::SetUpVector);
		script.AddReference("set_right", &MxObject::SetRightVector);
		script.AddReference("hide", &MxObject::Hide);
		script.AddReference("show", &MxObject::Show);
		script.AddReference("translation", &MxObject::GetTranslation);
		script.AddReference("rotation", &MxObject::GetRotation);
		script.AddReference("scale", &MxObject::GetScale);

		// Application
		script.AddVariable("objects", Application::Get()->objects);
		script.AddVariable("viewport", Application::Get()->GetRenderer().ViewPort);
		script.AddReference("load", Application::Get(), &Application::CreateObject);
		script.AddReference("copy", Application::Get(), &Application::CopyObject);
		script.AddReference("delete", Application::Get(), &Application::DestroyObject);
		script.AddReference("exit", Application::Get(), &Application::CloseApplication);
		script.AddReference("dt", Application::Get(), &Application::GetTimeDelta);
		script.AddReference("[]",
			[](Application::ObjectStorage& storage, const std::string& name) -> MxObject&
			{
				return *storage.Get()[name];
			});
		script.AddReference("to_string",
			[](const Vector3& vec)
			{
				return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
			});
		script.AddReference("set_texture",
			[](MxObject& instance, const std::string& path)
			{
				instance.Texture = Application::Get()->CreateTexture(path);
			});
		script.AddReference("set_shader",
			[](MxObject& instance, const std::string& vertex, const std::string& fragment)
			{
				instance.Shader = Application::Get()->CreateShader(vertex, fragment);
			});
		script.AddReference("remove_event",
			[](const std::string& name)
			{
				Application::Get()->GetEventDispatcher().RemoveEventListener(name);
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
				Application::Get()->GetEventDispatcher().AddEventListener<UpdateEvent>(eventName,
					[name = eventName, func = std::move(func)](UpdateEvent&)
				{
					try
					{
						func();
					}
					catch (std::exception & e)
					{
						std::string error = e.what();
						auto it = error.find("Error:");
						if (it != error.npos)
						{
							error.erase(it, it + 6);
							error = "[error]:" + error;
						}
						Application::Get()->Console.Log(error);
						Application::Get()->GetEventDispatcher().RemoveEventListener(name);
					}
				});
			});
	}
	#elif defined(MXENGINE_USE_PYTHON)
	void Application::CreateConsoleBindings(DeveloperConsole& console)
	{
		console.SetSize({ this->GetWindow().GetWidth() / 2.5f, this->GetWindow().GetHeight() / 2.0f });
		this->GetEventDispatcher().AddEventListener<RenderEvent>("DeveloperConsole",
			[this](RenderEvent&) { this->GetConsole().OnRender(); });
	}
	#else
	void Application::CreateConsoleBindings(DeveloperConsole& console)
	{
		console.SetSize({ this->GetWindow().GetWidth() / 2.5f, this->GetWindow().GetHeight() / 2.0f });
		this->GetEventDispatcher().AddEventListener<RenderEvent>("DeveloperConsole",
			[this](RenderEvent&) { this->GetConsole().OnRender(); });
	}
	#endif
}