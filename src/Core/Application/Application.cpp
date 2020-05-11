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
#include "Core/Event/Event.h"
#include "Core/Camera/PerspectiveCamera.h"
#include "Core/Camera/OrthographicCamera.h"

// conditional includes
#include "Core/Macro/Macro.h"

#include "Platform/OpenGL/GraphicFactory/GLGraphicFactory.h"
#include "Library/Scripting/Python/PythonEngine.h"
#include "Library/Primitives/Colors.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
	template<typename EventType>
	void ForwardEvent(const std::string& name)
	{
		Application::Get()->GetEventDispatcher().AddEventListener(name, [](EventType& e)
		{
			Application::Get()->GetCurrentScene().GetEventDispatcher().Invoke(e);
		});
	}

	void InitEventDispatcher()
	{
		ForwardEvent<AppDestroyEvent>  ("MxAppDestroyEvent");
		ForwardEvent<FpsUpdateEvent>   ("MxFpsUpdateEvent");
		ForwardEvent<WindowResizeEvent>("MxWindowResizeEvent");
		ForwardEvent<MouseMoveEvent>   ("MxMouseMoveEvent");
		ForwardEvent<RenderEvent>      ("MxRenderEvent"); 
		ForwardEvent<UpdateEvent>      ("MxUpdateEvent"); 
		ForwardEvent<KeyEvent>         ("MxKeyEvent");
	}

	size_t ComputeLODLevel(const MxObject& object, const CameraController& camera)
	{
		const auto& box = object.GetAABB();
		float distance  = Length(box.GetCenter() - camera.GetPosition());
		Vector3 length = box.Length();
		float maxLength = Max(length.x, length.y, length.z);
		float scaledDistance = maxLength / distance / camera.GetZoom();

		constexpr static std::array lodDistance = {
				0.21f,
				0.15f,
				0.10f,
				0.06f,
				0.03f,
		};
		size_t lod = 0;
		while (lod < lodDistance.size() && scaledDistance < lodDistance[lod]) 
			lod++;

		return lod;
	}

	Application::Application()
		: manager(this), window(Graphics::Instance()->CreateWindow(1600, 900, "MxEngine Application")),
		timeDelta(0), counterFPS(0), renderer(Graphics::Instance()->GetRenderer())
	{
		this->GetWindow().UseEventDispatcher(&this->dispatcher);
		this->CreateScene("Global", MakeUnique<Scene>("Global", "Resources/"));
		this->CreateScene("Default", MakeUnique<Scene>("Default", "Resources/"));

		InitEventDispatcher();
	}

	void Application::ToggleDebugDraw(bool aabb, bool spheres, const Vector4& color, bool overlay)
	{
		this->debugColor = color;
		this->drawBoxes = aabb;
		this->drawSpheres = spheres;
		this->overlayDebug = overlay;
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

	Scene& Application::GetCurrentScene()
	{
		return *this->currentScene;
	}

	Scene& Application::GetGlobalScene()
	{
		MX_ASSERT(this->scenes.Exists("Global"));
		return *this->scenes.Get("Global");
	}

	void Application::LoadScene(const std::string& name)
	{
		if (name == this->GetGlobalScene().GetName())
		{
			Logger::Instance().Error("MxEngine::Application", "global scene cannot be loaded: " + name);
			return;
		}
		if (!this->scenes.Exists(name))
		{
			Logger::Instance().Error("MxEngine::Application", "cannot load scene as it does not exist: " + name);
			return;
		}
		// unload previous scene if it exists
		if (this->currentScene != nullptr)
			this->currentScene->OnUnload();

		this->currentScene = this->scenes.Get(name);
		this->currentScene->OnLoad();
	}

	void Application::DestroyScene(const std::string& name)
	{
		if (name == this->GetGlobalScene().GetName())
		{
			Logger::Instance().Error("MxEngine::Application", "trying to destroy global scene: " + name);
			return;
		}
		if (!this->scenes.Exists(name))
		{
			Logger::Instance().Warning("MxEngine::Application", "trying to destroy not existing scene: " + name);
			return;
		}
		if (this->scenes.Get(name) == this->currentScene)
		{
			Logger::Instance().Error("MxEngine::Application", "cannot destroy scene which is used: " + name);
			return;
		}
		this->scenes.Delete(name);
	}

	Scene& Application::CreateScene(const std::string& name, UniqueRef<Scene> scene)
	{
		if (scenes.Exists(name))
		{
			Logger::Instance().Error("MxEngine::Application", "scene with such name already exists: " + name);
		}
		else
		{
			scenes.Add(name, std::move(scene));
			scenes.Get(name)->OnCreate();
		}
		return *scenes.Get(name);
	}

	Scene& Application::GetScene(const std::string& name)
	{
		MX_ASSERT(this->scenes.Exists(name));
		return *this->scenes.Get(name);
	}

	bool Application::SceneExists(const std::string& name)
	{
		return this->scenes.Exists(name);
	}

	Counter::CounterType Application::GenerateResourceId()
	{
		return this->resourceIdCounter++;
	}

	float Application::GetTimeDelta() const
	{
		return this->timeDelta;
	}

	int Application::GetCurrentFPS() const
	{
		return this->counterFPS;
	}

	void Application::SetMSAASampling(size_t samples)
	{
		this->GetEventDispatcher().AddEventListener("MSAAfactorTrack", [](WindowResizeEvent& e)
			{
				auto& renderer = Application::Get()->GetRenderer();
				renderer.SetMSAASampling(renderer.getMSAASamples(), (int)e.New.x, (int)e.New.y);
			});
		this->GetRenderer().SetMSAASampling(Max(1, samples), this->GetWindow().GetWidth(), this->GetWindow().GetHeight());
	}

	AppEventDispatcher& Application::GetEventDispatcher()
	{
		return this->dispatcher;
	}

	RenderController& Application::GetRenderer()
	{
		return this->renderer;
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

	void Application::ExecuteScript(const std::string& script)
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

	void Application::ToggleLighting(bool state)
	{
		if (state != this->drawLighting)
		{
			this->drawLighting = state;
			this->renderer.ObjectShader = nullptr;
			this->VerifyRendererState();
		}
	}

	void Application::DrawObjects()
	{
		MAKE_SCOPE_PROFILER("Application::DrawObjects");
		const auto& viewport = this->currentScene->Viewport;

		LightSystem lights;
		if (this->drawLighting)
		{
			lights.Global = &this->currentScene->GlobalLight;
			lights.Point = this->currentScene->PointLights.GetView();
			lights.Spot = this->currentScene->SpotLights.GetView();

			VerifyLightSystem(lights);

			this->renderer.ToggleReversedDepth(false);
			this->renderer.ToggleDepthOnlyMode(true);

			// compute LOD for each object
			for (const auto& [name, object] : this->currentScene->GetObjectList())
			{
				if (object->GetMesh() != nullptr && object->UseLOD && object->GetInstanceCount() == 0)
				{
					object->GetMesh()->SetLOD(std::numeric_limits<size_t>::max()); // set lowest LOD firstly
				}
			}
			for (const auto& [name, object] : this->currentScene->GetObjectList())
			{
				// we do not try to compute LOD for instanced objects, as it is potentially more resource-demanding, then rendering
				// better to let user himself specify desired LOD. TODO: consider optimizing LOD computing for instances
				if (object->GetMesh() != nullptr && object->UseLOD && object->GetInstanceCount() == 0)
				{
					// as objects can use same mesh, find best LOD for all of them. TODO: compute LOD per object, but only once per frame
					object->GetMesh()->SetLOD(Min(object->GetMesh()->GetLOD(), ComputeLODLevel(*object, viewport)));
				}
			}

			// first draw global directional light
			{
				lights.Global->ProjectionCenter = viewport.GetPosition();
				this->renderer.AttachDepthTexture(*lights.Global->GetDepthTexture());
				MAKE_SCOPE_PROFILER("Renderer::DrawGlobalLightDepthTexture");
				for (const auto& [name, object] : this->currentScene->GetObjectList())
				{
					this->renderer.DrawDepthTexture(*object, *lights.Global);
				}
			}

			// then draw spot lights
			for (const auto& spotLight : lights.Spot)
			{
				MAKE_SCOPE_PROFILER("Renderer::DrawSpotLightDepthTexture");
				this->renderer.AttachDepthTexture(*spotLight.GetDepthTexture());
				for (const auto& object : this->currentScene->GetObjectList())
				{
					this->renderer.DrawDepthTexture(*object.second, spotLight);
				}
			}

			// after draw point lights
			for (const auto& pointLight : lights.Point)
			{
				MAKE_SCOPE_PROFILER("Renderer::DrawPointLightDepthCubeMap");
				this->renderer.AttachDepthCubeMap(*pointLight.GetDepthCubeMap());
				for (const auto& object : this->currentScene->GetObjectList())
				{
					this->renderer.DrawDepthCubeMap(*object.second, pointLight, pointLight.FarDistance);
				}
			}

			this->renderer.SetViewport(0, 0, this->window->GetWidth(), this->window->GetHeight());
			this->renderer.DetachDepthBuffer();
			this->renderer.ToggleReversedDepth(true);
		}

		this->renderer.ToggleDepthOnlyMode(false);
		this->renderer.Clear();
		this->renderer.AttachDrawBuffer();

		if (this->drawLighting)
		{
			// now draw the scene as usual (with all framebuffers)
			{
				MAKE_SCOPE_PROFILER("Renderer::DrawScene");
				for (const auto& [name, object] : this->currentScene->GetObjectList())
				{
					this->renderer.DrawObject(*object, viewport, lights, this->currentScene->SceneSkybox.get());
				}
			}
		}
		else // no lighting, no shadows
		{
			MAKE_SCOPE_PROFILER("Renderer::DrawScene");
			for (const auto& [name, object] : this->currentScene->GetObjectList())
			{
				this->renderer.DrawObject(*object, viewport);
			}
		}

		{
			MAKE_SCOPE_PROFILER("Renderer::DrawSkybox");
			auto& skybox = this->GetCurrentScene().SceneSkybox;
			if (skybox != nullptr) this->renderer.DrawSkybox(*skybox, viewport);
		}

		if (this->drawBoxes | this->drawSpheres)
		{
			auto& buffer = this->renderer.GetDebugBuffer();
			for (const auto& [name, object] : this->currentScene->GetObjectList())
			{
				AABB box;
				if (object->GetInstanceCount() > 0 && object->GetMesh() != nullptr)
				{
					for (const auto& instance : object->GetInstances())
					{
						box = object->GetMesh()->GetAABB() * instance.Model.GetMatrix();
						if (this->drawBoxes) 
							buffer.SubmitAABB(box, debugColor);
						if (this->drawSpheres) 
							buffer.SubmitSphere(ToSphere(box), debugColor);
					}
				}
				else
				{
					box = object->GetAABB();
					if (this->drawBoxes) 
						buffer.SubmitAABB(box, debugColor);
					if (this->drawSpheres) 
						buffer.SubmitSphere(ToSphere(box), debugColor);
				}
			}
			this->renderer.DrawDebugBuffer(viewport, this->overlayDebug);
		}

		this->renderer.DetachDrawBuffer();
	}

	void Application::InvokeUpdate()
	{
		this->GetWindow().OnUpdate();
		MAKE_SCOPE_PROFILER("MxEngine::OnUpdate");
		UpdateEvent updateEvent(this->timeDelta);
		this->GetEventDispatcher().Invoke(updateEvent);
		{
			MAKE_SCOPE_PROFILER("Scene::OnUpdate");
			for (auto& [_, object] : this->currentScene->GetObjectList())
			{
				object->OnUpdate();
			}
			this->currentScene->OnUpdate();
		}
		{
			MAKE_SCOPE_PROFILER("Application::OnUpdate");
			this->OnUpdate();
		}
		{
			MAKE_SCOPE_PROFILER("Scene::PrepareRender");
			this->currentScene->OnRender();
			this->currentScene->PrepareRender();
		}
	}

	bool Application::VerifyApplicationState()
	{
		if (!this->GetWindow().IsCreated())
		{
			Logger::Instance().Error("MxEngine::Application", "window was not created, aborting...");
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

	void Application::VerifyRendererState()
	{
		auto& Renderer = this->GetRenderer();
		auto& GlobalScene = this->GetGlobalScene();
		if (Renderer.DefaultTexture == nullptr)
		{
			Renderer.DefaultTexture = Colors::MakeTexture(Colors::WHITE);
		}
		if (Renderer.DefaultNormal == nullptr)
		{
			Renderer.DefaultNormal = Colors::MakeTexture(Colors::FLAT_NORMAL);
		}
		if (Renderer.DefaultHeight == nullptr)
		{
			Renderer.DefaultHeight = Colors::MakeTexture(Colors::GREY);
		}
		if (Renderer.ObjectShader == nullptr)
		{
			if (this->drawLighting)
			{
				Renderer.ObjectShader = GlobalScene.GetResourceManager<Shader>().Add(
					"MxObjectShader", Graphics::Instance()->CreateShader());
				Renderer.ObjectShader->LoadFromString(
					#include MAKE_PLATFORM_SHADER(object_vertex)
					,
					#include MAKE_PLATFORM_SHADER(object_fragment)
				);
			}
			else
			{
				Renderer.ObjectShader = GlobalScene.GetResourceManager<Shader>().Add(
					"MxNoLightShader", Graphics::Instance()->CreateShader());
				Renderer.ObjectShader->LoadFromString(
					#include MAKE_PLATFORM_SHADER(nolight_object_vertex)
					,
					#include MAKE_PLATFORM_SHADER(nolight_object_fragment)
				);
			}
		}
		if (Renderer.DepthTextureShader == nullptr)
		{
			Renderer.DepthTextureShader = GlobalScene.GetResourceManager<Shader>().Add(
				"MxDepthTextureShader", Graphics::Instance()->CreateShader());
			Renderer.DepthTextureShader->LoadFromString(
				#include MAKE_PLATFORM_SHADER(depthtexture_vertex)
				,
				#include MAKE_PLATFORM_SHADER(depthtexture_fragment)
			);
		}
		if (Renderer.DepthCubeMapShader == nullptr)
		{
			Renderer.DepthCubeMapShader = GlobalScene.GetResourceManager<Shader>().Add(
				"MxDepthCubeMapShader", Graphics::Instance()->CreateShader());
			Renderer.DepthCubeMapShader->LoadFromString(
				#include MAKE_PLATFORM_SHADER(depthcubemap_vertex)
				,
				#include MAKE_PLATFORM_SHADER(depthcubemap_geometry)
				,
				#include MAKE_PLATFORM_SHADER(depthcubemap_fragment)
			);
		}
		if (Renderer.MSAAShader == nullptr)
		{
			Renderer.MSAAShader = GlobalScene.GetResourceManager<Shader>().Add(
				"MxMSAAShader", Graphics::Instance()->CreateShader());
			Renderer.MSAAShader->LoadFromString(
				#include MAKE_PLATFORM_SHADER(rect_vertex)
				,
				#include MAKE_PLATFORM_SHADER(msaa_fragment)
			);
		}
		if (Renderer.HDRShader == nullptr)
		{
			Renderer.HDRShader = GlobalScene.GetResourceManager<Shader>().Add(
				"MxHDRShader", Graphics::Instance()->CreateShader());
			Renderer.HDRShader->LoadFromString(
				#include MAKE_PLATFORM_SHADER(rect_vertex)
				,
				#include MAKE_PLATFORM_SHADER(hdr_fragment)
			);
		}
		if (Renderer.BloomShader == nullptr)
		{
			Renderer.BloomShader = GlobalScene.GetResourceManager<Shader>().Add(
				"MxBloomShader", Graphics::Instance()->CreateShader());
			Renderer.BloomShader->LoadFromString(
				#include MAKE_PLATFORM_SHADER(rect_vertex)
				,
				#include MAKE_PLATFORM_SHADER(bloom_fragment)
			);
		}
		if (Renderer.UpscaleShader == nullptr)
		{
			Renderer.UpscaleShader = GlobalScene.GetResourceManager<Shader>().Add(
				"MxUpscaleShader", Graphics::Instance()->CreateShader());
			Renderer.UpscaleShader->LoadFromString(
				#include MAKE_PLATFORM_SHADER(rect_vertex)
				,
				#include MAKE_PLATFORM_SHADER(upscale_fragment)
			);
		}

		auto& skybox = this->GetCurrentScene().SceneSkybox;
		if (skybox == nullptr) skybox = MakeUnique<Skybox>();

		if (Renderer.DepthBuffer == nullptr)
		{
			Renderer.DepthBuffer = GlobalScene.GetResourceManager<FrameBuffer>().Add(
				"MxDepthBuffer", Graphics::Instance()->CreateFrameBuffer());
		}
	}

	void Application::VerifyLightSystem(LightSystem& lights)
	{
		auto dirBufferSize = (int)this->renderer.GetDepthBufferSize<DirectionalLight>();
		auto spotBufferSize = (int)this->renderer.GetDepthBufferSize<SpotLight>();
		auto pointBufferSize = (int)this->renderer.GetDepthBufferSize<PointLight>();

		if (lights.Global->GetDepthTexture() == nullptr ||
			lights.Global->GetDepthTexture()->GetWidth() != dirBufferSize)
		{
			auto depthTexture = Graphics::Instance()->CreateTexture();
			depthTexture->LoadDepth(dirBufferSize, dirBufferSize);
			lights.Global->AttachDepthTexture(std::move(depthTexture));
		}

		for (auto& spotLight : lights.Spot)
		{
			if (spotLight.GetDepthTexture() == nullptr ||
				spotLight.GetDepthTexture()->GetWidth() != spotBufferSize)
			{
				auto depthTexture = Graphics::Instance()->CreateTexture();
				depthTexture->LoadDepth(spotBufferSize, spotBufferSize);
				spotLight.AttachDepthTexture(std::move(depthTexture));
			}
		}

		for (auto& pointLight : lights.Point)
		{
			if (pointLight.GetDepthCubeMap() == nullptr ||
				pointLight.GetDepthCubeMap()->GetWidth() != pointBufferSize)
			{
				auto depthCubeMap = Graphics::Instance()->CreateCubeMap();
				depthCubeMap->LoadDepth(pointBufferSize, pointBufferSize);
				pointLight.AttachDepthCubeMap(std::move(depthCubeMap));
			}
		}
	}

	void Application::CloseApplication()
	{
		this->shouldClose = true;
	}

	void Application::CreateContext()
	{
		#if defined(MXENGINE_DEBUG)
		bool useDebugging = true;
		#else
		bool useDebugging = false;
		#endif


		if (this->GetWindow().IsCreated())
		{
			Logger::Instance().Warning("MxEngine::Application", "CreateContext() called when window was already created");
			return;
		}
		MAKE_SCOPE_PROFILER("Application::CreateContext");
		this->GetWindow()
			.UseProfile(4, 0, Profile::CORE)
			.UseCursorMode(CursorMode::DISABLED)
			.UseDoubleBuffering(false)
			.UseTitle("MxEngine Project")
			.UseDebugging(useDebugging)
			.UsePosition(300, 150)
			.Create();

		auto& renderingEngine = this->renderer.GetRenderEngine();
		renderingEngine
			.UseCulling()
			.UseDepthBuffer()
			.UseLineWidth(3)
			.UseReversedDepth(false)
			.UseClearColor(0.0f, 0.0f, 0.0f, 1.0f)
			.UseBlending(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA)
			.UseAnisotropicFiltering(renderingEngine.GetLargestAnisotropicFactor())
			;

		this->CreateConsoleBindings(this->GetConsole());
		this->SetMSAASampling(4);
		this->renderer.SetBloomIterations(5);
	}

	DeveloperConsole& Application::GetConsole()
	{
		return this->console;
	}

	void Application::Run()
	{
		if (!VerifyApplicationState()) return;
		this->isRunning = true;

		if (this->GetConsole().IsToggled())
		{
			this->GetConsole().Log("Welcome to MxEngine developer console!");
			#if defined(MXENGINE_USE_PYTHON)
			this->GetConsole().Log("This console is powered by Python: https://www.python.org");
			#endif
		}

		{
			MAKE_SCOPE_PROFILER("Application::OnCreate");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::OnCreate()");
			this->LoadScene("Default");
			this->OnCreate();
		}
		float secondEnd = Time::Current(), frameEnd = Time::Current();
		int fpsCounter = 0;
		VerifyRendererState();
		{
			MAKE_SCOPE_PROFILER("Application::Run");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::Run()");
			Logger::Instance().Debug("MxEngine::Application", "starting main loop...");
			while (this->GetWindow().IsOpen())
			{
				fpsCounter++;
				float now = Time::Current();
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
					this->currentScene->GetEventDispatcher().InvokeAll();
					if (this->shouldClose) break;
				}

				this->InvokeUpdate();
				this->DrawObjects();

				RenderEvent renderEvent;
				this->GetEventDispatcher().Invoke(renderEvent);
				this->renderer.Render();
				this->GetWindow().PullEvents();
				if (this->shouldClose) break;
			}

			// application exit
			{
				MAKE_SCOPE_PROFILER("Application::CloseApplication");
				MAKE_SCOPE_TIMER("MxEngine::Application", "Application::CloseApplication()");
				this->currentScene->OnUnload();
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
		{
			MAKE_SCOPE_PROFILER("Application::DestroyObjects");
			MAKE_SCOPE_TIMER("MxEngine::Application", "Application::DestroyObjects");

			for (auto& scene : this->scenes.GetStorage())
			{
				scene.second->OnDestroy();
			}
			this->scenes.Clear();
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
		#if defined(MXENGINE_DEBUG)
		Profiler::Instance().StartSession("profile_log.json");
		#endif

		MX_ASSERT(Application::Get() == nullptr);
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
#if defined(MXENGINE_DEBUG)
		Profiler::Instance().EndSession();
#endif
	}

#if defined(MXENGINE_USE_PYTHON)
	void Application::CreateConsoleBindings(DeveloperConsole& console)
	{
		console.SetSize({ this->GetWindow().GetWidth() / 2.5f, this->GetWindow().GetHeight() / 2.0f });
		this->GetEventDispatcher().AddEventListener("DeveloperConsole",
			[this](RenderEvent&) { this->GetConsole().OnRender(); });
	}
#else
	void Application::CreateConsoleBindings(DeveloperConsole& console)
	{
		console.SetSize({ this->GetWindow().GetWidth() / 2.5f, this->GetWindow().GetHeight() / 2.0f });
		this->GetEventDispatcher().AddEventListener("DeveloperConsole",
			[this](RenderEvent&) { this->GetConsole().OnRender(); });
	}
#endif
}