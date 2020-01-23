#include "IApplication.h"
#include "Utilities/Logger/Logger.h"
#include "Core/ChaiScript/ChaiScriptUtils.h"
#include "Utilities/Math/Math.h"

#undef GetObject

namespace MomoEngine
{
	IApplication::IApplication()
		: window(1280, 720), TimeDelta(0), CounterFPS(0)
	{
		this->chaiScript = new chaiscript::ChaiScript();
		// intialize chaiscript
		this->Console.SetChaiScriptObject(this->chaiScript);

		//////////////////////// chai script bindings ///////////////////////////////////
		ChaiScriptApplication::Init(*this->chaiScript, this);
		ChaiScriptGLInstance::Init(*this->chaiScript);
		ChaiScriptRenderer::Init(*this->chaiScript);
		ChaiScriptCamera::Init(*this->chaiScript);
		/////////////////////////////////////////////////////////////////////////////////
		window.UseEventDispatcher(&this->Dispatcher);
	}

	Window& IApplication::GetWindow()
	{
		return this->window;
	}

	TimeStep IApplication::GetTimeDelta() const
	{
		return this->TimeDelta;
	}

	void IApplication::SetWindow(Window window)
	{
		this->window = std::move(window);
		this->window.UseEventDispatcher(&this->Dispatcher);
	}

	RendererImpl& IApplication::GetRenderer()
	{
		return Renderer::Instance();
	}

	GLInstance& IApplication::CreateObject(const std::string& name, const std::string& path)
	{
		if (this->objects.find(name) != this->objects.end())
		{
			Logger::Instance().Warning("MomoEngine::IApplication", "overriding already existing object: " + name);
			DestroyObject(name);
		}
		this->objects.insert({ name, GLInstance(MakeRef<GLObject>(this->ResourcePath + path)) });
		return GetObject(name);
	}

	GLInstance& IApplication::GetObject(const std::string& name)
	{
		if (this->objects.find(name) == this->objects.end())
		{
			Logger::Instance().Warning("MomoEngine::IApplication", "object was not found: " + name);
			return this->defaultInstance;
		}
		return this->objects[name];
	}

	void IApplication::DestroyObject(const std::string& name)
	{
		if (this->objects.find(name) == this->objects.end())
		{
			Logger::Instance().Warning("MomoEngine::IApplication", "trying to destroy object which not exists: " + name);
			return;
		}
		this->objects.erase(name);
	}

	void IApplication::DrawObjects(bool meshes) const
	{
		if (meshes)
		{
			for (const auto& object : this->objects)
			{
				Renderer::Instance().Draw(object.second);
				Renderer::Instance().DrawObjectMesh(object.second);
			}
		}
		else
		{
			for (const auto& object : this->objects)
			{
				Renderer::Instance().Draw(object.second);
			}
		}
	}

	void IApplication::CloseApplication()
	{
		this->OnDestroy();
		this->window.Close();
	}

	void IApplication::CreateDefaultContext()
	{
		this->window
			.UseProfile(3, 3, Profile::CORE)
			.UseCursorMode(CursorMode::DISABLED)
			.UseSampling(4)
			.UseDoubleBuffering(false)
			.UseTitle("MomoEngine Project")
			.UsePosition(600, 300)
			.Create();

		this->GetRenderer()
			.UseAnisotropicFiltering(this->GetRenderer().GetLargestAnisotropicFactor())
			.UseDepthBuffer()
			.UseCulling()
			.UseSampling()
			.UseTextureMagFilter(MagFilter::NEAREST)
			.UseTextureMinFilter(MinFilter::LINEAR_MIPMAP_LINEAR)
			.UseTextureWrap(WrapType::REPEAT, WrapType::REPEAT)
			.UseBlending(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA)
			.UseClearColor(0.0f, 0.0f, 0.0f)
			.UseImGuiStyle(ImguiStyle::DARK);
	}

	AppEventDispatcher& IApplication::GetEventDispatcher()
	{
		return this->Dispatcher;
	}

	void IApplication::Run()
	{
		TimeStep initStart = Time::Current();
		Logger::Instance().Debug("MomoEngine::Application", "calling Application::OnCreate()");
		this->OnCreate();
		TimeStep initEnd = Time::Current();
		auto time = BeautifyTime(initEnd - initStart);
		Logger::Instance().Debug("MomoEngine::Application", "Application::OnCreate returned in " + time);

		float secondEnd = Time::Current(), frameEnd = Time::Current();
		int fpsCounter = 0;

		Logger::Instance().Debug("MomoEngine::Application", "starting main loop...");
		while (this->window.IsOpen())
		{
			fpsCounter++;
			float now = window.GetTime();
			if (now - secondEnd >= 1.0f)
			{
				this->CounterFPS = fpsCounter;
				fpsCounter = 0;
				secondEnd = now;
			}
			TimeDelta = now - frameEnd;
			frameEnd = now;

			// event phase
			this->Dispatcher.InvokeAll();
			if (!this->window.IsOpen()) break;

			this->window.OnUpdate();

			float onUpdateStart = Time::Current();
			this->OnUpdate();
			if (!this->window.IsOpen()) break;
			float onUpdateEnd = Time::Current();
			if (onUpdateEnd - onUpdateStart > 0.01666f)
			{
				if (onUpdateEnd - onUpdateStart > 0.03333f)
					Logger::Instance().Error("MomoEngine::Application", "Application::OnUpdate running more than 33.33ms");
				else
					Logger::Instance().Warning("MomoEngine::Application", "Application::OnUpdate running more than 16.66ms");
			}

			this->GetRenderer().Flush();
			this->window.PullEvents();
		}
	}

	IApplication::~IApplication()
	{
		this->GetRenderer().DefaultTexture.reset();
		this->GetRenderer().MeshShader.reset();
		this->GetRenderer().ObjectShader.reset();

		delete this->chaiScript;

		Logger::Instance().Debug("MomoEngine::Application", "application destroyed");
	}

	void ChaiScriptApplication::Init(chaiscript::ChaiScript& chai, IApplication* app)
	{
		chai.add_global(chaiscript::var(std::ref(app->objects)), "objects");
		chai.add_global(chaiscript::var(std::ref(Renderer::Instance().ViewPort)), "view");

		chai.add(chaiscript::fun(&IApplication::CreateObject, app), "load");
		chai.add(chaiscript::fun(&IApplication::DestroyObject, app), "delete");
		chai.add(chaiscript::fun(
			[](IApplication::ObjectStorage& storage, const std::string& name) -> GLInstance&
			{
				return storage[name];
			}), "[]");
		chai.add(chaiscript::fun(
			[](const Vector3& vec)
			{
				return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
			}), "to_string");

		chai.add(chaiscript::fun(
			[&res = app->ResourcePath](GLInstance& instance, const std::string& path)
		{
			instance.Texture = MakeRef<Texture>(res + path);
		}), "set_texture");

		chai.add(chaiscript::fun(
			[&res = app->ResourcePath](GLInstance& instance, const std::string& vertex, const std::string& fragment)
		{
			instance.Shader = MakeRef<Shader>(res + vertex, res + fragment);
		}), "set_shader");
	}
}