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

// event system
#include "Core/Events/Events.h"
#include "Core/Application/Event.h"

// utilities
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/Json/Json.h"
#include "Utilities/ImGui/Editors/ComponentEditor.h"
#include "Utilities/Format/Format.h"

// components
#include "Core/Components/Components.h"

namespace MxEngine
{
	Application::Application()
		: manager(this), window(MakeUnique<Window>(1600, 900, "MxEngine Application"))
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

	size_t Application::GetCurrentFPS() const
	{
		return this->counterFPS;
	}

	EventDispatcher& Application::GetEventDispatcher()
	{
		return this->dispatcher;
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
		this->GetWindow().UseEventDispatcher(isPolled ? &this->dispatcher : nullptr);
	}

	void Application::CloseOnKeyPress(KeyCode key)
	{
		MXLOG_INFO("MxEngine::AppCloseBinding", MxFormat("bound app close to keycode: {0}", EnumToString(key)));
		Event::AddEventListener("AppCloseEvent", [key](KeyEvent& event)
		{
			auto context = Application::Get();
			if (event.IsHeld(key))
			{
				context->CloseApplication();
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
			PhysicsModule::OnUpdate(this->timeDelta);
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
		FileManager::SetRoot(ToFilePath(config.ProjectRootDirectory));

		this->GetWindow()
			.UseEventDispatcher(&this->dispatcher)
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
			.UseLineWidth(this->config.DebugLineWidth)
			.UseClearColor(0.0f, 0.0f, 0.0f, 1.0f)
			.UseBlending(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA)
			.UseAnisotropicFiltering((float)this->config.AnisothropicFiltering)
			;

		this->GetRuntimeEditor().AddKeyBinding(config.EditorOpenKey);
		this->InitializeRuntime(this->GetRuntimeEditor());
		this->CloseOnKeyPress(config.ApplicationCloseKey);
		this->InitializeRenderAdaptor(this->GetRenderAdaptor());
	}

	RuntimeEditor& Application::GetRuntimeEditor()
	{
		return this->console;
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

		float secondEnd = Time::Current();
		float frameEnd  = Time::Current();
		size_t frameCount = 0;
		{
			MAKE_SCOPE_PROFILER("Application::Run");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::Run()");
			MXLOG_INFO("MxEngine::Application", "starting main loop...");

			while (this->GetWindow().IsOpen()) //-V807
			{
				this->UpdateTimeDelta(frameEnd, secondEnd, frameCount);
				this->InvokeUpdate();
				this->DrawObjects();
				this->GetWindow().PullEvents();
				if (this->shouldClose) break;
			}

			// application exit
			{
				MAKE_SCOPE_PROFILER("Application::CloseApplication");
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
		#if defined(MXENGINE_PROFILING_ENABLED)
		Profiler::Start("profile_log.json");
		#endif

		MX_ASSERT(Application::Get() == nullptr);
		Application::Set(app);

		Logger::Init();
		FileManager::Init();
		AudioModule::Init();
		GraphicModule::Init();
		PhysicsModule::Init();
		UUIDGenerator::Init();
		GraphicFactory::Init();
		ComponentFactory::Init();
		ResourceFactory::Init();
		AudioFactory::Init();
		PhysicsFactory::Init();
		MxObject::Factory::Init();
	}

	Application::ModuleManager::~ModuleManager()
	{
		PhysicsModule::Destroy();
		GraphicModule::Destroy();
		AudioFactory::DeInit(); // OpenAL is angry when buffers are not deleted
		AudioModule::Destroy();

		#if defined(MXENGINE_PROFILING_ENABLED)
		Profiler::Finish();
		#endif
	}

	void Application::InitializeRenderAdaptor(RenderAdaptor& adaptor)
	{
		adaptor.InitRendererEnvironment();
	}

	void Application::UpdateTimeDelta(TimeStep& lastFrameEnd, TimeStep& lastSecondEnd, size_t& framesPerSecond)
	{
		if (this->IsPaused)
		{
			lastFrameEnd = 0.0f;
			lastSecondEnd = 0.0f;
			framesPerSecond = 0;
			this->counterFPS = 0;
			this->timeDelta = 0.0f;
			return;
		}

		float currentTime = Time::Current();
		framesPerSecond++;
		// check if 1 second passed. If so, update current FPS counter and add event
		if (lastFrameEnd - lastSecondEnd >= 1.0f)
		{
			this->counterFPS = framesPerSecond;
			lastSecondEnd = currentTime;
			framesPerSecond = 0;
			Event::AddEvent(MakeUnique<FpsUpdateEvent>(this->counterFPS));
		}
		// limit dt to be not less than 30fps
		this->timeDelta = this->TimeScale * Min(currentTime - lastFrameEnd, 1.0f / 30.0f);
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
		auto& editor = this->GetRuntimeEditor();

		editor.Log("Welcome to MxEngine developer console!");
		#if defined(MXENGINE_USE_PYTHON)
		editor.Log("This console is powered by Python: https://www.python.org");
		#endif

		editor.ExecuteScript("InitializeOpenGL()");

		editor.RegisterComponentEditor("Behaviour",          GUI::BehaviourEditor);
		editor.RegisterComponentEditor("Script",             GUI::ScriptEditor);
		editor.RegisterComponentEditor("InstanceFactory",    GUI::InstanceFactoryEditor);
		editor.RegisterComponentEditor("Instance",           GUI::InstanceEditor);
		editor.RegisterComponentEditor("Skybox",             GUI::SkyboxEditor);
		editor.RegisterComponentEditor("DebugDraw",          GUI::DebugDrawEditor);
		editor.RegisterComponentEditor("MeshRenderer",       GUI::MeshRendererEditor);
		editor.RegisterComponentEditor("MeshSource",         GUI::MeshSourceEditor);
		editor.RegisterComponentEditor("MeshLOD",            GUI::MeshLODEditor);
		editor.RegisterComponentEditor("DirectionalLight",   GUI::DirectionalLightEditor);
		editor.RegisterComponentEditor("PointLight",         GUI::PointLightEditor);
		editor.RegisterComponentEditor("SpotLight",          GUI::SpotLightEditor);
		editor.RegisterComponentEditor("CameraController",   GUI::CameraControllerEditor);
		editor.RegisterComponentEditor("VRCameraController", GUI::VRCameraControllerEditor);
		editor.RegisterComponentEditor("InputControl",       GUI::InputControlEditor);
		editor.RegisterComponentEditor("AudioSource",        GUI::AudioSourceEditor);
		editor.RegisterComponentEditor("AudioListener",      GUI::AudioListenerEditor);
		editor.RegisterComponentEditor("RigidBody",          GUI::RigidBodyEditor);
		editor.RegisterComponentEditor("BoxCollider",        GUI::BoxColliderEditor);
		editor.RegisterComponentEditor("SphereCollider",     GUI::SphereColliderEditor);
		editor.RegisterComponentEditor("CylinderCollider",   GUI::CylinderColliderEditor);
		editor.RegisterComponentEditor("CapsuleCollider",    GUI::CapsuleColliderEditor);

		this->RegisterComponentUpdate<Behaviour>();
		this->RegisterComponentUpdate<InstanceFactory>();
		this->RegisterComponentUpdate<VRCameraController>();
		this->RegisterComponentUpdate<AudioListener>();
		this->RegisterComponentUpdate<AudioSource>();
		this->RegisterComponentUpdate<RigidBody>();
	}
}