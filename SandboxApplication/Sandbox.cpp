#include "Sandbox.h"
#include <Library/Bindings/InputBinding.h>

#include <algorithm>
#include <array>

MxEngine::Application* MxEngine::GetApplication()
{
	return new SandboxApp();
}

SandboxApp::SandboxApp()
{
	this->CreateContext();
	this->ResourcePath = "Resources/";
}

void SandboxApp::OnCreate()
{
	/*
    auto& DestroyerObject = CreateObject("Destroyer", "objects/destroyer/destroyer.obj");
	auto& DeathStarObject = CreateObject("DeathStar", "objects/death_star/death_star.obj");

	DestroyerObject.Texture = CreateTexture("objects/destroyer/Aluminm5.jpg");
	DeathStarObject.Texture = CreateTexture("objects/death_star/texture.jpg");

    DeathStarObject
        .Scale(0.00005f)
        .RotateX(-90.0f)
        .RotateZ(-90.0f)
        .Translate(-10.0f, 10.0f, 10.0f);

    DestroyerObject
        .Translate(3.0f, 1.0f, 2.0f) // to origin
        .Translate(-5, 0.0f, 5.0f)
        .Scale(0.0005f);
	*/
	auto& ArcObject  = CreateObject("Arc170", "objects/arc170/arc170.obj");
	auto& CubeObject = CreateObject("Cube", "objects/crate/crate.obj");
	auto& GridObject = CreateObject("Grid", "objects/grid/grid.obj");
	
	GridObject.Shader = CreateShader("shaders/grid_vertex.glsl", "shaders/grid_fragment.glsl");
	
	this->GetRenderer().DefaultTexture = CreateTexture("textures/default.jpg");
	this->GetRenderer().ObjectShader = CreateShader("shaders/object_vertex.glsl", "shaders/object_fragment.glsl");
	this->GetRenderer().MeshShader = CreateShader("shaders/mesh_vertex.glsl", "shaders/mesh_fragment.glsl");
	
	int cubeCount = 100;
    CubeObject
        .Translate(0.5f, 0.5f, 0.5f)
        .AddInstanceBuffer([](int idx, int coord)
            {
                return std::array<float, 3>{ 5.0f * std::sin(0.2f * idx), 0.5f * idx, 5.0f * std::cos(0.2f * idx) }[coord];
            }, 3, cubeCount);

	ArcObject
		.Scale(0.005f)
		.Translate(10.0f, 0.5f, -10.0f);
	
	auto camera = MakeUnique<PerspectiveCamera>();
	//auto camera = MakeUnique<OrthographicCamera>();

	camera->SetZFar(100000.0f);
	camera->SetAspectRatio((float)this->GetWindow().GetWidth(), (float)this->GetWindow().GetHeight());
	
	auto& controller = this->GetRenderer().ViewPort;
	controller.SetCamera(std::move(camera));
	controller.Translate(1.0f, 3.0f, 0.0f);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);

	InputBinding("ViewPortControl", this->GetRenderer().ViewPort)
		.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
		.BindRotation();
}

void SandboxApp::OnUpdate()
{
	static MxEngine::TimeStep timePassed = 0.0f;
	static bool drawMesh = false;

	auto& CubeObject      = GetObject("Cube");
	auto& ArcObject       = GetObject("Arc170");
	auto& GridObject      = GetObject("Grid");
	//auto& DestroyerObject = GetObject("Destroyer");
	//auto& DeathStarObject = GetObject("DeathStar");

	auto& window = this->GetWindow();
	if (window.GetCursorMode() != CursorMode::NORMAL)
	{
		if (window.IsKeyHeld(KeyCode::ESCAPE))
		{
			this->CloseApplication();
            return;
		}
	}
	if (window.IsKeyPressed(KeyCode::GRAVE_ACCENT))
	{
		static Vector2 cursorPos = window.GetCursorPos();
		if (window.GetCursorMode() == CursorMode::NORMAL)
		{
			window.UseCursorMode(CursorMode::DISABLED);
            this->ToggleDeveloperConsole(false);
			InputBinding("ViewPortControl", this->GetRenderer().ViewPort)
				.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
				.BindRotation();
			window.UseCursorPos(cursorPos);
		}
		else
		{
			cursorPos = window.GetCursorPos();
			window.UseCursorMode(CursorMode::NORMAL);
            this->ToggleDeveloperConsole(true);
			window.UseCursorPos({ window.GetWidth() / 4.0f, window.GetHeight() / 2.0f });
			this->GetEventDispatcher().RemoveEventListener("ViewPortControl");
		}
	}

	timePassed += this->TimeDelta;
	if (timePassed > 1.0f)
	{
		this->GetWindow().UseTitle("Sandbox App " + std::to_string(this->CounterFPS) + " FPS");
		timePassed = 0.0f;
	}

	if(this->GetWindow().GetCursorMode() == CursorMode::NORMAL)
	{
		auto& camera = this->GetRenderer().ViewPort;
		auto pos = camera.GetPosition();
		float speed = camera.GetMoveSpeed();
		auto zoom = camera.GetZoom();

		ImGui::SetNextWindowPos({ 0.0f, this->Console.GetSize().y });
		ImGui::SetNextWindowSize({ this->Console.GetSize().x, 0.0f });
		ImGui::Begin("Fast Game Editor");

		ImGui::Checkbox("display mesh", &drawMesh);
		ImGui::Text("speed: %f");
		ImGui::Text("position: (%f, %f, %f)", pos.x, pos.y, pos.z);

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
	const float deltaRot = 2.0f * this->TimeDelta;
	ArcObject.RotateY(deltaRot);

	this->GetRenderer().Clear();
	this->DrawObjects(drawMesh);
}

void SandboxApp::OnDestroy()
{

}