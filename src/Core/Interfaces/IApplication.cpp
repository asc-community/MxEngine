#include "IApplication.h"
#include "Utilities/Logger/Logger.h"

namespace MomoEngine
{

	IApplication::IApplication()
		: Window(1280, 720), TimeDelta(0), CounterFPS(0)
	{
	}

	RendererImpl& IApplication::GetRenderer()
	{
		return Renderer::Instance();
	}

	void IApplication::CloseApplication()
	{
		this->OnDestroy();
		this->Window.Close();
	}

	void IApplication::Run()
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
			.UseDepthBuffer()
			.UseCulling()
			.UseSampling()
			.UseTextureMagFilter(MagFilter::NEAREST)
			.UseTextureMinFilter(MinFilter::LINEAR_MIPMAP_LINEAR)
			.UseClearColor(0.0f, 0.0f, 0.0f);
		
		TimeStep initStart = Window.GetTime();
		Logger::Instance().Debug("MomoEngine::Application", "calling Application::OnCreate()");
		this->OnCreate();
		TimeStep initEnd = Window.GetTime();
		auto time = BeautifyTime(initEnd - initStart);
		Logger::Instance().Debug("MomoEngine::Application", "Application::OnCreate returned in " + time);

		float secondEnd = 0.0f, frameEnd = 0.0f;
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

			this->GetRenderer().Clear();
			this->OnUpdate();
			this->GetRenderer().Finish();
			this->Window.PullEvents();
		}
	}
}