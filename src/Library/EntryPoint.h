#pragma once

#ifdef MOMO_ENGINE_INCLUDE

namespace MxEngine
{
	extern Application* __cdecl GetApplication();
}

namespace
{
	extern "C"
	{
		int main()
		{
			MxEngine::Application* app = MxEngine::GetApplication();
			app->Run();
			delete app;
			return 0;
		}
	}
}
#endif