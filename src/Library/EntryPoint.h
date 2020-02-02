#pragma once

#ifdef MOMO_ENGINE_INCLUDE
namespace MomoEngine
{
	extern Application* __cdecl GetApplication();
}

namespace
{
	extern "C"
	{
		int main()
		{
			MomoEngine::Application* app = MomoEngine::GetApplication();
			app->Run();
			delete app;
			return 0;
		}
	}
}
#endif