// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
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
#include "Platform/GraphicAPI.h"
#include "Platform/Modules/GraphicModule.h"
#include "Core/Event/Event.h"

// conditional includes
#include "Core/Macro/Macro.h"

#include "Library/Scripting/Python/PythonEngine.h"
#include "Library/Primitives/Colors.h"

#include "Utilities/Logger/Logger.h"
#include "Utilities/Math/Math.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/UUID/UUID.h"
#include "Utilities/Json/Json.h"
#include "Utilities/ECS/ComponentFactory.h"

#include "Core/Components/Behaviour.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshLOD.h"

namespace MxEngine
{
	Application::Application()
		: manager(this), window(MakeUnique<Window>(1600, 900, "MxEngine Application"))
	{
		this->GetWindow().UseEventDispatcher(&this->dispatcher);
	}

	void Application::OnCreate()
	{
		//is overriden in derived class
	}

	void Application::OnUpdate()
	{
		// is overriden in derived class
	}

	void Application::OnDestroy()
	{
		// is overriden in derived class
	}

	Window& Application::GetWindow()
	{
		return *this->window;
	}

	float Application::GetTimeDelta() const
	{
		return this->timeDelta;
	}

	int Application::GetCurrentFPS() const
	{
		return this->counterFPS;
	}

	AppEventDispatcher& Application::GetEventDispatcher()
	{
		return this->dispatcher;
	}

	RenderAdaptor& Application::GetRenderAdaptor()
	{
		return this->renderAdaptor;
	}

	LoggerImpl& Application::GetLogger()
	{
		return Logger::Instance();
	}

	void Application::ExecuteScript(Script& script)
	{
		script.UpdateContents();
		this->ExecuteScript(script.GetContent());
	}

	void Application::ExecuteScript(const MxString& script)
	{
		this->ExecuteScript(script.c_str());
	}

	void Application::ExecuteScript(const char* script)
	{
		MAKE_SCOPE_PROFILER("Application::ExecuteScript");
		auto& engine = this->GetConsole().GetEngine();
		engine.Execute(script);
		if (engine.HasErrors())
		{
			Logger::Instance().Error("Application::ExecuteScript", engine.GetErrorMessage());
		}
	}

	void Application::ToggleDeveloperConsole(bool isVisible)
	{
		this->GetConsole().Toggle(isVisible);
		this->GetWindow().UseEventDispatcher(isVisible ? nullptr : &this->dispatcher);
	}

	void Application::DrawObjects()
	{
		MAKE_SCOPE_PROFILER("Application::DrawObjects");
		this->GetRenderAdaptor().PerformRenderIteration();
	}

	void Application::InvokeUpdate()
	{
		this->GetWindow().OnUpdate();
		MAKE_SCOPE_PROFILER("MxEngine::OnUpdate");
		UpdateEvent updateEvent(this->timeDelta);
		this->GetEventDispatcher().Invoke(updateEvent);
		{
			MAKE_SCOPE_PROFILER("MxObjects::OnUpdate");
			auto objects = MxObject::GetObjects();
			for (auto& object : objects)
			{
				auto behaviour = object.GetComponent<Behaviour>();
				if (behaviour.IsValid()) behaviour.GetUnchecked()->InvokeUpdate(timeDelta);
				auto instances = object.GetComponent<InstanceFactory>();
				if (instances.IsValid()) object.BufferInstances();
			}
		}
		{
			MAKE_SCOPE_PROFILER("Application::OnUpdate");
			this->OnUpdate();
		}
	}

	bool Application::VerifyApplicationState()
	{
		if (!this->GetWindow().IsCreated())
		{
			Logger::Instance().Error("MxEngine::Application", "window was not created, probably CreateContext() was not called");
			return false;
		}
		if (this->isRunning)
		{
			Logger::Instance().Error("MxEngine::Application", "Application::Run() is called when application is already running");
			return false;
		}
		if (!this->GetWindow().IsOpen())
		{
			Logger::Instance().Error("MxEngine::Application", "window was created but is closed. Note that application can be runned only once");
			return false;
		}
		return true; // verified!
	}

	void Application::CloseApplication()
	{
		this->shouldClose = true;
	}

	void Application::CreateContext()
	{
		if (this->GetWindow().IsCreated())
		{
			Logger::Instance().Warning("MxEngine::Application", "CreateContext() called when window was already created");
			return;
		}
		MAKE_SCOPE_PROFILER("Application::CreateContext");

		#if defined(MXENGINE_DEBUG)
		bool useDebugging = true;
		#else
		bool useDebugging = false;
		#endif

		JsonFile config;

		auto configPathHash = STRING_ID("engine_config.json");
		if (FileManager::FileExists(configPathHash))
		{
			Logger::Instance().Debug("Application::CreateContext", "Using engine config file to set up context...");
			File configFile(FileManager::GetFilePath(configPathHash));
			config = Json::LoadJson(configFile);
		}
		else
		{
			Logger::Instance().Warning("Application::CreateContext", "Engine config file was not provided, using default settings...");
			config["renderer"]["opengl-profile"] = "core";
			config["renderer"]["opengl-major-version"] = 4;
			config["renderer"]["opengl-minor-version"] = 0;
			config["renderer"]["msaa-samples"] = 4;
			config["renderer"]["anisothropic-filtering"] = 16;
			config["renderer"]["debug-line-width"] = 3;

			config["window"]["title"] = "MxEngine Application";
			config["window"]["double-buffering"] = false;
			config["window"]["position"] = std::array<int, 2> { 300, 150 };
			config["window"]["Size"] = std::array<int, 2> { 1600, 900 };
			config["window"]["cursor-mode"] = "disabled";
		}

		auto profileType  = config["renderer"]["opengl-profile"].get<std::string>();
		auto profileMajor = config["renderer"]["opengl-major-version"].get<int>();
		auto profileMinor = config["renderer"]["opengl-minor-version"].get<int>();
		auto msaaSamples  = config["renderer"]["msaa-samples"].get<int>();
		auto anisothropic = config["renderer"]["anisothropic-filtering"].get<int>();
		auto lineWidth    = config["renderer"]["debug-line-width"].get<int>();

		auto title        = config["window"]["title"].get<std::string>();
		auto doubleBuffer = config["window"]["double-buffering"].get<bool>();
		auto position     = config["window"]["position"].get<std::array<int, 2>>();
		auto size         = config["window"]["size"].get<std::array<int, 2>>();
		auto cursorMode   = config["window"]["cursor-mode"].get<std::string>();

		Profile enumProfile;
		if (profileType == "core")
			enumProfile = Profile::CORE;
		else if (profileType == "compat")
			enumProfile = Profile::COMPAT;
		else
			enumProfile = Profile::ANY;

		auto enumCursor = CursorMode::DISABLED;
		if (cursorMode == "disabled")
			enumCursor = CursorMode::DISABLED; //-V1048
		else if (cursorMode == "hidden")
			enumCursor = CursorMode::HIDDEN;

		this->GetWindow()
			.UseProfile(profileMajor, profileMinor, enumProfile)
			.UseCursorMode(CursorMode::DISABLED)
			.UseDoubleBuffering(doubleBuffer)
			.UseTitle(ToMxString(title))
			.UseDebugging(useDebugging)
			.UsePosition(position[0], position[1])
			.UseSize(size[0], size[1])
			.Create();

		auto& renderingEngine = this->GetRenderAdaptor().Renderer.GetRenderEngine();
		renderingEngine
			.UseCulling()
			.UseDepthBuffer()
			.UseLineWidth(lineWidth)
			.UseReversedDepth(false)
			.UseClearColor(0.0f, 0.0f, 0.0f, 1.0f)
			.UseBlending(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA)
			.UseAnisotropicFiltering(static_cast<float>(anisothropic))
			;

		this->InitializeDeveloperConsole(this->GetConsole());
		this->InitializeRenderAdaptor(this->GetRenderAdaptor());
	}

	DeveloperConsole& Application::GetConsole()
	{
		return this->console;
	}

	void Application::Run()
	{
		if (!VerifyApplicationState()) return;
		this->isRunning = true;

		this->GetConsole().Log("Welcome to MxEngine developer console!");
		#if defined(MXENGINE_USE_PYTHON)
		this->GetConsole().Log("This console is powered by Python: https://www.python.org");
		#endif

		{
			MAKE_SCOPE_PROFILER("Application::OnCreate");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::OnCreate()");
			this->OnCreate();
		}

		float secondEnd = Time::Current(), frameEnd = secondEnd;
		int fpsCounter = 0;
		{
			MAKE_SCOPE_PROFILER("Application::Run");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::Run()");
			Logger::Instance().Debug("MxEngine::Application", "starting main loop...");
			while (this->GetWindow().IsOpen())
			{
				fpsCounter++;
				const float now = Time::Current();
				if (now - secondEnd >= 1.0f)
				{
					this->counterFPS = fpsCounter;
					fpsCounter = 0;
					secondEnd = now;
					this->GetEventDispatcher().AddEvent(MakeUnique<FpsUpdateEvent>(this->counterFPS));
				}
				timeDelta = now - frameEnd;
				frameEnd = now;

				// event phase
				{
					MAKE_SCOPE_PROFILER("Application::ProcessEvents");
					this->GetEventDispatcher().InvokeAll();
					if (this->shouldClose) break;
				}

				this->InvokeUpdate();
				this->DrawObjects();

				RenderEvent renderEvent;
				this->GetEventDispatcher().Invoke(renderEvent);
				this->GetRenderAdaptor().Renderer.Render();
				this->GetWindow().PullEvents();
				if (this->shouldClose) break;
			}

			// application exit
			{
				MAKE_SCOPE_PROFILER("Application::CloseApplication");
				MAKE_SCOPE_TIMER("MxEngine::Application", "Application::CloseApplication()");
				AppDestroyEvent appDestroyEvent;
				this->GetEventDispatcher().Invoke(appDestroyEvent);
				this->OnDestroy();
				this->GetWindow().Close();
				this->isRunning = false;
			}
		}
	}

	bool Application::IsRunning() const
	{
		return this->isRunning;
	}

	Application::~Application()
	{
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
		#if defined(MXENGINE_DEBUG)
		Profiler::Instance().StartSession("profile_log.json");
		#endif

		MX_ASSERT(Application::Get() == nullptr);
		Application::Set(app);

		FileManager::Init("Resources");
		GraphicModule::Init();
		UUIDGenerator::Init();
		GraphicFactory::Init();
		ComponentFactory::Init();
		ResourceFactory::Init();
		MxObject::Factory::Init();
	}

	Application::ModuleManager::~ModuleManager()
	{
		GraphicModule::Destroy();
		#if defined(MXENGINE_DEBUG)
		Profiler::Instance().EndSession();
		#endif
	}

	void Application::InitializeRenderAdaptor(RenderAdaptor& adaptor)
	{
		adaptor.InitRendererEnvironment();
		this->GetRenderAdaptor().OnWindowResize({ this->GetWindow().GetWidth(), this->GetWindow().GetHeight() });
		this->GetEventDispatcher().AddEventListener("RenderAdaptorOnResize", [this](WindowResizeEvent& e)
		{
			this->GetRenderAdaptor().OnWindowResize(e.New);
		});
	}

#if defined(MXENGINE_USE_PYTHON)
	void Application::InitializeDeveloperConsole(DeveloperConsole& console)
	{
		console.SetSize({ 
			static_cast<float>(this->GetWindow().GetWidth()) / 2.5f, 
			static_cast<float>(this->GetWindow().GetHeight()) / 2.0f });
		this->GetEventDispatcher().AddEventListener("DeveloperConsole",
			[this](RenderEvent&) { this->GetConsole().OnRender(); });
		this->GetConsole().GetEngine().Execute("InitializeOpenGL()");
	}
#else
	void Application::InitializeDeveloperConsole(DeveloperConsole& console)
	{
		console.SetSize({ this->GetWindow().GetWidth() / 2.5f, this->GetWindow().GetHeight() / 2.0f });
		this->GetEventDispatcher().AddEventListener("DeveloperConsole",
			[this](RenderEvent&) { this->GetConsole().OnRender(); });
	}
#endif
}