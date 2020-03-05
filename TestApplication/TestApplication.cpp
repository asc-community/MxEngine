#include "TestApplication.h"
#include <Library/Bindings/Bindings.h>
#include <Library/Primitives/Primitives.h>

void TestApp::OnCreate()
{
	// add objects here
	this->AddObject("Grid", MakeUnique<Grid>());
	this->InvalidateObjects();

	auto& Renderer = this->GetRenderer();

	Renderer.DefaultTexture = Colors::MakeTexture(Colors::WHITE);
	Renderer.ObjectShader = this->CreateShader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
	Renderer.MeshShader = this->CreateShader("shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl");
	
	Renderer.GlobalLight
		.UseAmbientColor ({ 0.1f, 0.1f, 0.1f })
		.UseDiffuseColor ({ 0.5f, 0.5f, 0.5f })
		.UseSpecularColor({ 1.0f, 1.0f, 1.0f });
	
	auto camera = MakeUnique<PerspectiveCamera>();
	camera->SetZFar(1000.0f);
	camera->SetAspectRatio((float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight());
	
	auto& controller = Renderer.ViewPort;
	controller.SetCamera(std::move(camera));
	controller.TranslateY(1.0f);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);
	InputBinding("CameraControl", Renderer.ViewPort)
		.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
		.BindRotation();
}

void TestApp::OnUpdate()
{

}

void TestApp::OnDestroy()
{

}

TestApp::TestApp()
{
	this->ResourcePath = "D:/repos/MxEngine/TestApplication/Resources/";
	this->CreateContext();
}