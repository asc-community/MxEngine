#include "Sandbox.h"
#include "Gameplay/SandboxScene.h"

void SandboxApp::OnCreate()
{
	Event::AddEventListener<FpsUpdateEvent>("CountFPS",
		[this](auto& e)
		{
			WindowManager::SetTitle(MxFormat("Sandbox App {0} FPS", e.FPS));
		});
	scene->OnCreate();
}

void SandboxApp::OnUpdate()
{
	scene->OnUpdate();
}

void SandboxApp::OnDestroy()
{

}

SandboxApp::SandboxApp()
{
	scene = new SandboxScene();
}