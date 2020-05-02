#include "Sandbox.h"
#include "Gameplay/SandboxScene.h"
#include <Library/Bindings/Bindings.h>
#include <Library/Primitives/Primitives.h>

void SandboxApp::OnCreate()
{
	auto scene = MakeUnique<SandboxScene>();
	this->CreateScene("Sandbox", std::move(scene));
	this->LoadScene("Sandbox");

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);

	this->GetEventDispatcher().AddEventListener("CountFPS",
		[this](FpsUpdateEvent& e)
		{
			this->GetWindow().UseTitle(Format("Sandbox App {0} FPS", e.FPS));
		});
}

void SandboxApp::OnUpdate()
{

}

void SandboxApp::OnDestroy()
{

}

SandboxApp::SandboxApp()
{
	this->CreateContext();
}