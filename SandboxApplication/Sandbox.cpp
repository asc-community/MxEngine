#include "Sandbox.h"
#include <Library/Bindings/Bindings.h>

#include "Objects/UserObjects.h"

void SandboxApp::OnCreate()
{
	this->AddObject("Cube",   MakeUnique<CubeObject>());
	this->AddObject("Arc170", MakeUnique<Arc170Object>());
	this->AddObject("Grid",   MakeUnique<GridObject>());
	//this->AddObject("Destroyer", MakeUnique<DestroyerObject>());
	//this->AddObject("DeathStar", MakeUnique<DeathStarObject>());

	this->GetRenderer().DefaultTexture = CreateTexture("textures/default.jpg");
	this->GetRenderer().ObjectShader = CreateShader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
	this->GetRenderer().MeshShader = CreateShader("shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl");
	
	auto camera = MakeUnique<PerspectiveCamera>();
	//auto camera = MakeUnique<OrthographicCamera>();
	
	camera->SetZFar(100000.0f);
	camera->SetAspectRatio((float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight());
	
	auto& controller = this->GetRenderer().ViewPort;
	controller.SetCamera(std::move(camera));
	controller.Translate(1.0f, 3.0f, 0.0f);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);
	InputBinding("CameraControl", this->GetRenderer().ViewPort)
		.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
		.BindRotation();

	this->GetEventDispatcher().AddEventListener<FpsUpdateEvent>("CountFPS",
		[&window = this->GetWindow()](FpsUpdateEvent& e)
		{
			window.UseTitle("Sandbox App " + std::to_string(e.FPS) + " FPS");
		});
}

void SandboxApp::OnUpdate()
{
	if(this->Console.IsToggled())
	{
		auto& camera = this->GetRenderer().ViewPort;
		auto pos = camera.GetPosition();
		float speed = camera.GetMoveSpeed();
		auto zoom = camera.GetZoom();
		bool mesh = false;

		ImGui::SetNextWindowPos({ 0.0f, this->Console.GetSize().y });
		ImGui::SetNextWindowSize({ this->Console.GetSize().x, 0.0f });
		ImGui::Begin("Fast Game Editor");

		ImGui::Checkbox("display mesh", &mesh);
		ImGui::Text("speed: %f");
		ImGui::Text("position: (%f, %f, %f)", pos.x, pos.y, pos.z);

		this->ToggleMeshDrawing(mesh);

		if (ImGui::InputFloat("set speed", &speed))
		{
			camera.SetMoveSpeed(speed);
		}
		if (ImGui::InputFloat3("set position", &pos[0]))
		{
			camera.SetPosition(pos);
		}

		ImGui::Text("zoom / fov: %f", zoom);
		if (ImGui::DragFloat("zoom", &zoom, 0.1f, 0.1f, 20.0f))
		{
			camera.SetZoom(zoom);
		}
		ImGui::End();
	}
}

void SandboxApp::OnDestroy()
{

}

SandboxApp::SandboxApp()
{
	this->ResourcePath = "Resources/";
	this->CreateContext();
}
