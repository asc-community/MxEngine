#pragma once

#include "Core/Window/Window.h"
#include "Core/Renderer/Renderer.h"
#include "Utilities/Time/Time.h"

namespace MomoEngine
{
	struct IApplication
	{
	protected:
		std::string ResourcePath;
		Window Window;
		TimeStep TimeDelta;
		int CounterFPS;
		IApplication();
		RendererImpl& GetRenderer();

		virtual void OnCreate () = 0;
		virtual void OnUpdate () = 0;
		virtual void OnDestroy() = 0;

		void CloseApplication();
	public:
		void Run();
		virtual ~IApplication() = default;
	};
}