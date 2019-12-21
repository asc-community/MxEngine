#pragma once

#include "MomoEngine.h"

#ifdef MOMO_ENGINE_INCLUDE
namespace MomoEngine
{
	extern IApplication* GetApplication();
}

int main()
{
	MomoEngine::IApplication* app = MomoEngine::GetApplication();
	app->Run();
	delete app;
	return 0;
}
#endif