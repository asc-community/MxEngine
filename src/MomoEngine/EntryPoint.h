#pragma once

#ifdef MOMO_ENGINE_INCLUDE
namespace MomoEngine
{
	extern IApplication* __cdecl GetApplication();
}

namespace
{
	extern "C"
	{
		int main()
		{
			MomoEngine::IApplication* app = MomoEngine::GetApplication();
			app->Run();
			delete app;
			return 0;
		}
	}
}
#endif