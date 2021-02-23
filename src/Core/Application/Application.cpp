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
#include "Core/Macro/Macro.h"

// platform modules and api functions
#include "Platform/GraphicAPI.h"
#include "Platform/Modules/GraphicModule.h"
#include "Platform/AudioAPI.h"
#include "Platform/Modules/AudioModule.h"
#include "Platform/PhysicsAPI.h"
#include "Platform/Modules/PhysicsModule.h"

// global serializer
#include "Core/Application/GlobalContextSerializer.h"

// event system
#include "Core/Events/Events.h"
#include "Core/Application/Event.h"

// utilities
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/Json/Json.h"
#include "Utilities/Format/Format.h"

// components
#include "Core/Components/Components.h"

// editor
#include "Core/Runtime/RuntimeEditor.h"

namespace MxEngine
{
	Application::Application()
		: manager(this), window(MakeUnique<Window>(1600, 900, "MxEngine Application")), 
		  dispatcher(Alloc<EventDispatcherImpl<EventBase>>()), editor(Alloc<RuntimeEditor>())
	{
		this->CreateContext();
	}

	void Application::OnCreate()
	{
		//is overriden in derived class
	}

	void Application::OnUpdate()
	{
		// is overriden in derived class
	}

	void Application::OnRender()
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

	float Application::GetUnscaledTimeDelta() const
	{
		return this->timeDelta / this->TimeScale;
	}

	TimeStep Application::GetTotalElapsedTime() const
	{
		return this->totalElapsedTime;
	}

	void Application::SetTotalElapsedTime(TimeStep time)
	{
		this->totalElapsedTime = Max(time, 0.0f);
	}

	size_t Application::GetCurrentFPS() const
	{
		return this->counterFPS;
	}

	void Application::AddCollisionEntry(const MxObject::Handle& object1, const MxObject::Handle& object2)
	{
		auto& [currentCollisions, previousCollisions] = this->collisions;
		currentCollisions.emplace_back(object1, object2);
	}

	EventDispatcherImpl<EventBase>& Application::GetEventDispatcher()
	{
		return *this->dispatcher;
	}

	RenderAdaptor& Application::GetRenderAdaptor()
	{
		return this->renderAdaptor;
	}

	void Application::ToggleRuntimeEditor(bool isVisible)
	{
		this->GetRuntimeEditor().Toggle(isVisible);
		this->ToggleWindowUpdates(!isVisible); // no updates if editor is enabled
	}

	void Application::ToggleWindowUpdates(bool isPolled)
	{
		this->GetWindow().UseEventDispatcher(isPolled ? this->dispatcher : nullptr);
	}

	void Application::CloseOnKeyPress(KeyCode key)
	{
		MXLOG_INFO("MxEngine::AppCloseBinding", MxFormat("bound app close to keycode: {0}", EnumToString(key)));
		Event::AddEventListener<KeyEvent>("AppCloseEvent", [key](auto& event)
		{
			if (event.IsPressed(key))
			{
				Application::GetImpl()->CloseApplication();
			}
		});
	}

	void Application::DrawObjects()
	{
		MAKE_SCOPE_PROFILER("Application::DrawObjects");
		this->GetRenderAdaptor().SetWindowSize({ this->GetWindow().GetWidth(), this->GetWindow().GetHeight() });
		this->GetRenderAdaptor().RenderFrame();

		// invoke render event and application main callback
		RenderEvent renderEvent;
		Event::Invoke(renderEvent);
		this->OnRender();

		this->GetRenderAdaptor().SubmitRenderedFrame();
	}

	void Application::InvokeUpdate()
	{
		// update window and keyboard state
		{
			MAKE_SCOPE_PROFILER("MxEngine::OnUpdate");
			this->GetWindow().OnUpdate();
		}

		// do not invoke any events of perform physics if application is paused
		if (!this->IsPaused)
		{
			// invoke all events from previous frame or invoked by fps update
			{
				MAKE_SCOPE_PROFILER("Application::ProcessEvents");
				Event::InvokeAll();
			}

			// update physics simulation
			this->InvokePhysics();
		}

		// update runtime editor
		this->GetRuntimeEditor().OnUpdate();

		// do not update components or call update callbacks if application is paused
		if (!IsPaused)
		{
			// invoke all components waiting for updates
			{
				MAKE_SCOPE_PROFILER("Application::UpdateComponents");
				for (const auto& callback : this->updateCallbacks)
				{
					callback(this->timeDelta);
				}
			}

			// invoke update event
			UpdateEvent updateEvent(this->timeDelta);
			Event::Invoke(updateEvent);

			// invoke main application update
			{
				MAKE_SCOPE_PROFILER("Application::OnUpdate");
				this->OnUpdate();
			}
		}
	}

	void Application::InvokePhysics()
	{
		PhysicsModule::OnUpdate(this->timeDelta);

		// TODO: refactor, move collision logic to separate function
		MAKE_SCOPE_PROFILER("Physics::InvokeCollionsCallbacks()");

		auto& [currentCollisions, previousCollisions] = this->collisions;
		std::sort(currentCollisions.begin(), currentCollisions.end());
		auto previousCollisionEntry = previousCollisions.begin();

		constexpr auto CollisionCallback = [](MxObject::Handle& object1, MxObject::Handle& object2, auto callbackMethod)
		{
			constexpr auto valid = [](MxObject::Handle& object) -> bool
			{
				return object.IsValid() && object->HasComponent<RigidBody>();
			};
			if (valid(object1) && valid(object2))
				std::invoke(callbackMethod, object1->GetComponent<RigidBody>(), *object1, *object2);
			if (valid(object2) && valid(object1))
				std::invoke(callbackMethod, object2->GetComponent<RigidBody>(), *object2, *object1);
		};

		for (auto it = currentCollisions.begin(); it != currentCollisions.end(); it++)
		{
			auto& [object1, object2] = *it;

			while (previousCollisionEntry != previousCollisions.end() && *previousCollisionEntry < *it)
			{
				auto& [obj1, obj2] = *previousCollisionEntry;
				CollisionCallback(obj1, obj2, &RigidBody::InvokeOnCollisionExitCallback);
				previousCollisionEntry++;
			}
			
			if (previousCollisionEntry == previousCollisions.end() || *previousCollisionEntry != *it)
				CollisionCallback(object1, object2, &RigidBody::InvokeOnCollisionEnterCallback);

			if (previousCollisionEntry != previousCollisions.end() && *previousCollisionEntry == *it) 
				previousCollisionEntry++;

			CollisionCallback(object1, object2, &RigidBody::InvokeOnCollisionCallback);
		}

		while (previousCollisionEntry != previousCollisions.end())
		{
			auto& [obj1, obj2] = *previousCollisionEntry;
			CollisionCallback(obj1, obj2, &RigidBody::InvokeOnCollisionExitCallback);
			previousCollisionEntry++;
		}

		std::swap(currentCollisions, previousCollisions);
		currentCollisions.clear();
	}

	void Application::InvokeCreate()
	{
		MAKE_SCOPE_PROFILER("Application::OnCreate");
		MAKE_SCOPE_TIMER("MxEngine::Application", "Application::OnCreate()");
		this->OnCreate();
	}

	bool Application::VerifyApplicationState()
	{
		if (!this->GetWindow().IsCreated())
		{
			MXLOG_ERROR("MxEngine::Application", "window was not created, probably CreateContext() was not called");
			return false;
		}
		if (this->isRunning)
		{
			MXLOG_ERROR("MxEngine::Application", "Application::Run() is called when application is already running");
			return false;
		}
		if (!this->GetWindow().IsOpen())
		{
			MXLOG_ERROR("MxEngine::Application", "window was created but is currently closed. Note that application can be runned only once");
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
			MXLOG_WARNING("MxEngine::Application", "CreateContext() called when window was already created");
			return;
		}
		MAKE_SCOPE_PROFILER("Application::CreateContext");

		this->InitializeConfig(this->config);

		this->GetWindow()
			.UseEventDispatcher(this->dispatcher)
			.UseProfile((int)this->config.GraphicAPIMajorVersion, (int)this->config.GraphicAPIMinorVersion, this->config.GraphicAPIProfile)
			.UseCursorMode(this->config.Cursor)
			.UseDoubleBuffering(this->config.DoubleBuffering)
			.UseTitle(this->config.WindowTitle)
			.UseDebugging(this->config.GraphicAPIDebug)
			.UseWindowPosition((int)this->config.WindowPosition.x, (int)this->config.WindowPosition.y)
			.UseWindowSize((int)this->config.WindowSize.x, (int)this->config.WindowSize.y)
			.Create();

		auto& renderingEngine = this->GetRenderAdaptor().Renderer.GetRenderEngine();
		renderingEngine
			.UseCulling()
			.UseDepthBuffer()
			.UseSeamlessCubeMaps()
			.UseClearColor(0.0f, 0.0f, 0.0f, 1.0f)
			.UseBlending(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA)
			.UseAnisotropicFiltering((float)this->config.AnisothropicFiltering);

		this->GetRuntimeEditor().AddKeyBinding(config.EditorOpenKey);
		this->InitializeRuntime(this->GetRuntimeEditor());
		this->CloseOnKeyPress(config.ApplicationCloseKey);
		this->InitializeRenderAdaptor(this->GetRenderAdaptor());
		this->InitializeShaderDebug();
	}

	RuntimeEditor& Application::GetRuntimeEditor()
	{
		return *this->editor;
	}

    Config& Application::GetConfig()
    {
		return this->config;
    }

	void Application::Run()
	{
		if (!VerifyApplicationState()) return;
		this->isRunning = true;
		this->InvokeCreate();

		float secondEnd = Time::EngineCurrent();
		float frameEnd  = Time::EngineCurrent();
		size_t frameCount = 0;
		{
			MAKE_SCOPE_PROFILER("Application::Run()");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::Run()");
			MXLOG_INFO("MxEngine::Application", "starting main loop...");

			while (this->GetWindow().IsOpen()) //-V807
			{
				MAKE_SCOPE_PROFILER("Application::Frame()");
				this->UpdateTimeDelta(frameEnd, secondEnd, frameCount);
				this->InvokeUpdate();
				this->DrawObjects();
				this->GetWindow().PullEvents();
				if (this->shouldClose) break;
			}

			// application exit
			{
				MAKE_SCOPE_PROFILER("Application::CloseApplication()");
				MAKE_SCOPE_TIMER("MxEngine::Application", "Application::CloseApplication()");
				AppDestroyEvent appDestroyEvent;
				Event::Invoke(appDestroyEvent);
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
		MXLOG_INFO("MxEngine::Application", "application destroyed");
	}

	void Application::Init()
	{
	}

	Application* Application::GetImpl()
	{
		return Application::Current;
	}

	void Application::Clone(Application* application)
	{
		Application::Current = application;
	}

	Application::ModuleManager::ModuleManager(Application* app)
	{
		#if defined(MXENGINE_PROFILING_ENABLED)
		Profiler::Start("profile_log.json");
		#endif

		Application::Current = app;
		GlobalContextSerializer::Initialize();
	}

	Application::ModuleManager::~ModuleManager()
	{
		PhysicsModule::Destroy();
		GraphicModule::Destroy();
		AudioFactory::Destroy(); // OpenAL is angry when buffers are not deleted
		AudioModule::Destroy();

		#if defined(MXENGINE_PROFILING_ENABLED)
		Profiler::Finish();
		#endif
	}

	void Application::InitializeRenderAdaptor(RenderAdaptor& adaptor)
	{
		adaptor.InitRendererEnvironment();
	}

	void Application::InitializeShaderDebug()
	{
		#if defined(MXENGINE_DEBUG)
		MAKE_SCOPE_PROFILER("Application::InitializeShaderDebug()");
		MAKE_SCOPE_TIMER("MxEngine::Application", "InitializeShaderDebug()");
		auto& shaders = this->GetRenderAdaptor().Renderer.GetEnvironment().Shaders;
		if (shaders.empty())
		{
			MXLOG_WARNING("Application::InitializeShaderDebug", "method is called before InitializeRenderAdaptor()");
		}
		auto shaderDirectory = (FileManager::GetWorkingDirectory() / ToFilePath(this->config.ShaderSourceDirectory)).lexically_normal();
		if (!File::Exists(shaderDirectory))
		{
			MXLOG_WARNING("Application::InitializeShaderDebug", "shader debug directory does not exists: " + ToMxString(shaderDirectory));
			return;
		}

		for (auto it = shaders.begin(); it != shaders.end(); it++)
		{
			this->GetRuntimeEditor().AddShaderUpdateListener(it->second, shaderDirectory);
		}
		#endif
	}

	void Application::UpdateTimeDelta(TimeStep& lastFrameEnd, TimeStep& lastSecondEnd, size_t& framesPerSecond)
	{
		// query platform time
		float currentTime = Time::EngineCurrent();

		if (this->IsPaused)
		{
			lastFrameEnd = 0.0f;
			lastSecondEnd = 0.0f;
			framesPerSecond = 0;
			this->counterFPS = 0;
			this->timeDelta = 0.0f;
		}
		else
		{
			framesPerSecond++;
			// check if 1 second passed. If so, update current FPS counter and add event
			if (lastFrameEnd - lastSecondEnd >= 1.0f)
			{
				this->counterFPS = framesPerSecond;
				lastSecondEnd = currentTime;
				framesPerSecond = 0;
				Event::AddEvent(MakeUnique<FpsUpdateEvent>(this->counterFPS));
			}

			this->timeDelta = this->TimeScale * (currentTime - lastFrameEnd);
			this->totalElapsedTime += this->timeDelta;
		}
		lastFrameEnd = currentTime;
	}

	void Application::InitializeConfig(Config& config)
	{
		MAKE_SCOPE_PROFILER("Application::InitializeConfig");
		MAKE_SCOPE_TIMER("MxEngine::Application", "Application::InitializeConfig");

		const FilePath configPath = FileManager::GetWorkingDirectory() / "engine_config.json";
		File file;
		JsonFile jsonConfig;

		// if no config file exists, use default settings and create new config file
		if (!File::Exists(configPath))
		{
			MXLOG_INFO("MxEngine::Application", "config file was not found, generating default one...");
			Serialize(jsonConfig, config);
			file.Open(configPath, File::WRITE);
			SaveJson(file, jsonConfig);
		}
		MXLOG_INFO("MxEngine::Application", "loading application config from " + ToMxString(configPath));
		file.Open(configPath, File::READ);
		jsonConfig = LoadJson(file);
		file.Close();

		if (!jsonConfig.empty())
		{
			Deserialize(config, jsonConfig);
		}
		else
		{
			MXLOG_ERROR("MxEngine::Application", "config was not loaded properly: " + ToMxString(configPath));
		}

		FileManager::InitializeRootDirectory(FileManager::GetWorkingDirectory());

		#if defined(MXENGINE_SHIPPING)
		config.GraphicAPIDebug = false;
		config.EditorOpenKey = KeyCode::UNKNOWN;
		config.ApplicationCloseKey = KeyCode::UNKNOWN;
		#endif

		#if defined(MXENGINE_SHIPPING)
		MXLOG_INFO("MxEngine::Application", "application is running in shipping mode");
		#elif defined(MXENGINE_RELEASE)
		MXLOG_INFO("MxEngine::Application", "application is running in release mode");
		#elif defined(MXENGINE_DEBUG)
		MXLOG_INFO("MxEngine::Application", "application is running in debug mode");
		#else
		MXLOG_WARNING("MxEngine::Application", "application is running in unknown mode");
		#endif
	}

	void Application::InitializeRuntime(RuntimeEditor& console)
	{
		// initialize runtime compiler
		this->GetEventDispatcher().AddEventListener<FpsUpdateEvent>("RuntimeCompiler",
			[](auto&) { RuntimeCompiler::OnUpdate(1.0f); });

		if (!this->config.AutoRecompileFiles)
		{
			RuntimeCompiler::ToggleAutoCompilation(false);
			this->GetEventDispatcher().AddEventListener<UpdateEvent>("RuntimeCompiler", 
				[timeSinceLastCompile = 0.0f](UpdateEvent& e) mutable
				{ 
					timeSinceLastCompile += e.TimeDelta;
					auto app = Application::GetImpl();
					auto key = app->GetConfig().RecompileFilesKey;
					if (timeSinceLastCompile > 5.0f && app->GetWindow().IsKeyHeldUnchecked(key))
					{
						RuntimeCompiler::StartCompilationTask();
						timeSinceLastCompile = 0.0f;
					}
				});
		}

		// initialize editor and component update callbacks
		auto& editor = this->GetRuntimeEditor();
		editor.LogToConsole("Welcome to MxEngine developer console!");

		// register all predefined components and their update methods
		MxEngine::RegisterComponents();
	}
}