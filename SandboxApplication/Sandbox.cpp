#include "Sandbox.h"
#include "Gameplay/SandboxScene.h"

void SandboxApp::OnCreate()
{
	EventManager::AddEventListener("CountFPS",
		[this](FpsUpdateEvent& e)
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