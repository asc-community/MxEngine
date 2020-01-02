#include "IApplication.h"
#include "Utilities/Logger/Logger.h"
#define CHAISCRIPT_NO_THREADS
#include <chaiscript/chaiscript.hpp>
#undef GetObject

namespace MomoEngine
{
	IApplication::IApplication()
		: Window(1280, 720), TimeDelta(0), CounterFPS(0)
	{
		this->chaiScript = new chaiscript::ChaiScript();
		// intialize chaiscript
		this->Console.SetChaiScriptObject(this->chaiScript);

		//////////////////////// chai script bindings ///////////////////////////////////
		this->chaiScript->add_global(chaiscript::var(std::ref(this->objects)), "objects");
		this->chaiScript->add_global(chaiscript::var(std::ref(Renderer::Instance().ViewPort)), "viewport");

		this->chaiScript->add(chaiscript::fun(&IApplication::CreateObject, this), "load");
		this->chaiScript->add(chaiscript::fun(&IApplication::DestroyObject, this), "delete");
		this->chaiScript->add(chaiscript::fun(
			[](ObjectStorage& storage, const std::string& name) -> GLInstance&
			{
				return storage[name];
			}), "[]");
		this->chaiScript->add(chaiscript::fun(
			[](const glm::vec3& vec)
			{
				return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
			}), "to_string");

		this->chaiScript->add(chaiscript::fun(
			[&res = this->ResourcePath](GLInstance& instance, const std::string& path)
			{
				instance.Texture = MakeRef<Texture>(res + path);
			}), "set_texture");

		this->chaiScript->add(chaiscript::fun(
			[&res = this->ResourcePath](GLInstance& instance, const std::string& vertex, const std::string& fragment)
			{
				instance.Shader = MakeRef<Shader>(res + vertex, res + fragment);
			}), "set_shader");
		this->chaiScript->add(chaiscript::fun(
			[](float zFar) { Renderer::Instance().ViewPort.GetCamera().SetZFar(zFar); }), "set_zfar");
		this->chaiScript->add(chaiscript::fun(
			[](float zNear) { Renderer::Instance().ViewPort.GetCamera().SetZNear(zNear); }), "set_znear");
		this->chaiScript->add(chaiscript::fun(
			[]() { return Renderer::Instance().ViewPort.GetCamera().GetZFar(); }), "zfar");
		this->chaiScript->add(chaiscript::fun(
			[]() { return Renderer::Instance().ViewPort.GetCamera().GetZNear(); }), "znear");

		#define CHAI_IMPORT(SIGNATURE, NAME) this->chaiScript->add(chaiscript::fun(SIGNATURE), #NAME)

		CHAI_IMPORT(&GLInstance::RotateX, rotate_x);
		CHAI_IMPORT(&GLInstance::RotateY, rotate_y);
		CHAI_IMPORT(&GLInstance::RotateZ, rotate_z);

		CHAI_IMPORT((GLInstance&(GLInstance::*)(float))&GLInstance::Scale, scale);
		CHAI_IMPORT((GLInstance&(GLInstance::*)(float, float, float))&GLInstance::Scale, scale);

		CHAI_IMPORT(&GLInstance::Translate, translate);
		CHAI_IMPORT(&GLInstance::TranslateX, translate_x);
		CHAI_IMPORT(&GLInstance::TranslateY, translate_y);
		CHAI_IMPORT(&GLInstance::TranslateZ, translate_z);

		CHAI_IMPORT(&GLInstance::Hide, hide);
		CHAI_IMPORT(&GLInstance::Show, show);

		CHAI_IMPORT(&GLInstance::GetTranslation, translation);
		CHAI_IMPORT(&GLInstance::GetRotation, rotation);
		CHAI_IMPORT(&GLInstance::GetScale, scale);

		CHAI_IMPORT(&CameraController::GetPosition,  position);
		CHAI_IMPORT(&CameraController::GetDirection, direction);
		CHAI_IMPORT(&CameraController::GetUpVector,  up);
		CHAI_IMPORT(&CameraController::GetZoom, zoom);
		CHAI_IMPORT(&CameraController::SetZoom, set_zoom);
		CHAI_IMPORT(&CameraController::Rotate, rotate);

		CHAI_IMPORT((CameraController&(CameraController::*)(float, float, float))&CameraController::Translate, translate);
		CHAI_IMPORT(&CameraController::TranslateX, translate_x);
		CHAI_IMPORT(&CameraController::TranslateY, translate_y);
		CHAI_IMPORT(&CameraController::TranslateZ, translate_z);
		CHAI_IMPORT(&CameraController::TranslateForward, move_forward);
		CHAI_IMPORT(&CameraController::TranslateRight, move_right);
		CHAI_IMPORT(&CameraController::TranslateUp, move_up);
		///////////////////////////////////////////////////////////////////////////////////////////
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
		this->Window.Close();
	}

	void IApplication::CreateDefaultContext()
	{
		this->Window
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
			.UseImGuiStyle(ImGuiStyle::DARK);
	}

	void IApplication::Run()
	{
		TimeStep initStart = Window.GetTime();
		Logger::Instance().Debug("MomoEngine::Application", "calling Application::OnCreate()");
		this->OnCreate();
		TimeStep initEnd = Window.GetTime();
		auto time = BeautifyTime(initEnd - initStart);
		Logger::Instance().Debug("MomoEngine::Application", "Application::OnCreate returned in " + time);

		float secondEnd = Window.GetTime(), frameEnd = Window.GetTime();
		int fpsCounter = 0;

		Logger::Instance().Debug("MomoEngine::Application", "starting main loop...");
		while (this->Window.IsOpen())
		{
			fpsCounter++;
			float now = Window.GetTime();
			if (now - secondEnd >= 1.0f)
			{
				this->CounterFPS = fpsCounter;
				fpsCounter = 0;
				secondEnd = now;
			}
			TimeDelta = now - frameEnd;
			frameEnd = now;

			float onUpdateStart = this->Window.GetTime();
			this->OnUpdate();
			float onUpdateEnd = this->Window.GetTime();
			if (onUpdateEnd - onUpdateStart > 16.66f)
			{
				if (onUpdateEnd - onUpdateStart > 33.33f)
					Logger::Instance().Error("MomoEngine::Application", "Application::OnUpdate running more than 33.33ms");
				else
					Logger::Instance().Warning("MomoEngine::Application", "Application::OnUpdate running more than 16.66ms");
			}

			this->GetRenderer().Finish();
			this->Window.PullEvents();
			this->GetRenderer().Clear();

		}
	}

	IApplication::~IApplication()
	{
		delete this->chaiScript;
	}
}