#include "Sandbox.h"
#include "Objects/UserObjects.h"
#include <Library/Bindings/Bindings.h>
#include <Library/Primitives/Primitives.h>

void SandboxApp::OnCreate()
{
	this->AddObject("Grid",   MakeUnique<Grid>());
	this->AddObject("Cube",   MakeUnique<CubeObject>());
	this->AddObject("Arc170", MakeUnique<Arc170Object>());
	this->AddObject("Sphere", MakeUnique<SphereObject>());
	// this->AddObject("Destroyer", MakeUnique<DestroyerObject>());
	// this->AddObject("DeathStar", MakeUnique<DeathStarObject>());
	this->InvalidateObjects();

	auto& Renderer = this->GetRenderer();

	Renderer.PointLights.SetCount(1);
	LightBinding(Renderer.PointLights).BindAll();
	Renderer.SpotLights.SetCount(1);
	LightBinding(Renderer.SpotLights).BindAll();

	Renderer.DefaultTexture = Colors::MakeTexture(Colors::WHITE);
	Renderer.ObjectShader = CreateShader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
	Renderer.MeshShader = CreateShader("shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl");
	
	Renderer.GlobalLight
		.UseAmbientColor ({ 0.0f, 0.0f, 0.0f })
		.UseDiffuseColor ({ 0.3f, 0.3f, 0.3f })
		.UseSpecularColor({ 1.0f, 1.0f, 1.0f });

	Renderer.PointLights[0]
		.UseAmbientColor ({ 1.0f, 0.3f, 0.0f })
		.UseDiffuseColor ({ 1.0f, 0.3f, 0.0f })
		.UseSpecularColor({ 1.0f, 0.3f, 0.0f })
		.UsePosition({ -3.0f, 2.0f, -3.0f });

	Renderer.SpotLights[0]
		.UseAmbientColor ({ 0.2f, 0.2f, 0.2f })
		.UseDiffuseColor ({ 0.5f, 0.5f, 0.5f })
		.UseSpecularColor({ 0.5f, 0.5f, 0.5f })
		.UsePosition({ -15.0f, 3.0f, 0.0f })
		.UseDirection({ -1.0f, 1.3f, -1.0f })
		.UseOuterAngle(35.0f)
		.UseInnerAngle(15.0f);
	
	auto camera = MakeUnique<PerspectiveCamera>();
	//auto camera = MakeUnique<OrthographicCamera>();
	
	camera->SetZFar(100000.0f);
	camera->SetAspectRatio((float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight());
	
	auto& controller = Renderer.ViewPort;
	controller.SetCamera(std::move(camera));
	controller.Translate(1.0f, 3.0f, 0.0f);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);
	InputBinding("CameraControl", Renderer.ViewPort)
		.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
		.BindRotation();

	this->GetEventDispatcher().AddEventListener<FpsUpdateEvent>("CountFPS",
		[this](FpsUpdateEvent& e)
		{
			this->GetWindow().UseTitle("Sandbox App " + std::to_string(e.FPS) + " FPS");
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
	this->ResourcePath = "Resources/";
	this->CreateContext();
}