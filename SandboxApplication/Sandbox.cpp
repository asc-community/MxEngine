#include "Sandbox.h"

#include <algorithm>
#include <array>

MomoEngine::IApplication* MomoEngine::GetApplication()
{
	return new SandboxApp();
}

std::string helloWorld(const std::string& str)
{
	Logger::Instance().Debug("ChaiScript", "Hello World!");
	return str;
}

SandboxApp::SandboxApp()
{
	CreateDefaultContext();
	this->ResourcePath = "Resources/";
}

void SandboxApp::OnCreate()
{
	/*
	auto& DestroyerObject = CreateObject("Destroyer", "objects/destroyer/destroyer.obj");
	auto& DeathStarObject = CreateObject("DeathStar", "objects/death_star/death_star.obj");

	DestroyerObject.Texture = MakeRef<Texture>(ResourcePath + "objects/destroyer/Aluminm5.jpg");
	DeathStarObject.Texture = MakeRef<Texture>(ResourcePath + "objects/death_star/texture.jpg");
	*/
	auto& DestroyerObject = GetObject("Destroyer");
	auto& DeathStarObject = GetObject("DeathStar");
	
	auto& ArcObject  = CreateObject("Arc170", "objects/arc170/arc170.obj");
	auto& CubeObject = CreateObject("Cube", "objects/crate/crate.obj");
	auto& GridObject = CreateObject("Grid", "objects/grid/grid.obj");
	
	GridObject.Shader = MakeRef<Shader>(ResourcePath + "shaders/grid_vertex.glsl", ResourcePath + "shaders/grid_fragment.glsl");
	
	this->GetRenderer().DefaultTexture = MakeRef<Texture>(ResourcePath + "textures/default.jpg");
	this->GetRenderer().ObjectShader = MakeRef<Shader>(ResourcePath + "shaders/object_vertex.glsl", ResourcePath + "shaders/object_fragment.glsl");
	this->GetRenderer().MeshShader = MakeRef<Shader>(ResourcePath + "shaders/mesh_vertex.glsl", ResourcePath + "shaders/mesh_fragment.glsl");
	
	int cubeCount = 100;
	CubeObject.AddInstanceBuffer([](int idx, int coord)
		{
			return std::array<float, 3>({ 5.0f * std::sin(0.2f * idx), 0.5f * idx, 5.0f * std::cos(0.2f * idx) })[coord];
		}, 3, cubeCount);

	ArcObject
		.Scale(0.005f)
		.Translate(10.0f, 0.0f, -10.0f);
	DeathStarObject
		.Scale(0.00005f)
		.RotateX(-90.0f)
		.RotateZ(-90.0f)
		.Translate(-10.0f, 10.0f, 10.0f);

	DestroyerObject
		.Translate(3.0f, 1.0f, 2.0f) // to origin
		.Translate(-5, 0.0f, 5.0f)
		.Scale(0.0005f);
	
	auto camera = MakeUnique<PerspectiveCamera>();
	//auto camera = MakeUnique<OrthographicCamera>();

	camera->SetZFar(100000.0f);
	camera->SetAspectRatio(Window.GetWidth(), Window.GetHeight());
	
	this->GetRenderer().ViewPort.SetCamera(std::move(camera));
	this->GetRenderer().ViewPort.Translate(1.0f, 3.0f, 0.0f);
}

void SandboxApp::OnUpdate()
{
	static MomoEngine::TimeStep timePassed = 0.0f;
	static Position cursorPos;
	static float speed = 5.0;
	static float mouseSpeed = 0.75f;
	static bool drawMesh = false;

	static auto& CubeObject      = GetObject("Cube");
	static auto& ArcObject       = GetObject("Arc170");
	static auto& GridObject      = GetObject("Grid");
	static auto& DestroyerObject = GetObject("Destroyer");
	static auto& DeathStarObject = GetObject("DeathStar");

	timePassed += this->TimeDelta;
	if (timePassed > 1.0f)
	{
		this->Window.UseTitle("Sandbox App " + std::to_string(this->CounterFPS) + " FPS");
		timePassed = 0.0f;
	}
	// event handling 
	{
		if (Window.GetCursorMode() != CursorMode::NORMAL)
		{
			auto curPos = Window.GetCursorPos();
			auto& camera = this->GetRenderer().ViewPort;
			camera.Rotate(
				mouseSpeed * this->TimeDelta * (cursorPos.x - curPos.x),
				mouseSpeed * this->TimeDelta * (cursorPos.y - curPos.y)
			);
			cursorPos = curPos;

			if (Window.IsKeyHolded(KeyCode::W))
			{
				camera.TranslateForward(this->TimeDelta * speed);
			}
			if (Window.IsKeyHolded(KeyCode::S))
			{
				camera.TranslateForward(-this->TimeDelta * speed);
			}
			if (Window.IsKeyHolded(KeyCode::D))
			{
				camera.TranslateRight(this->TimeDelta * speed);
			}
			if (Window.IsKeyHolded(KeyCode::A))
			{
				camera.TranslateRight(-this->TimeDelta * speed);
			}
			if (Window.IsKeyHolded(KeyCode::SPACE))
			{
				camera.TranslateUp(this->TimeDelta * speed);
			}
			if (Window.IsKeyHolded(KeyCode::LEFT_SHIFT))
			{
				camera.TranslateUp(-this->TimeDelta * speed);
			}
			if (Window.IsKeyPressed(KeyCode::ESCAPE))
			{
				this->CloseApplication(); 
				return;
			}
		}
		if (Window.IsKeyPressed(KeyCode::GRAVE_ACCENT))
		{
			if (this->Window.GetCursorMode() == CursorMode::NORMAL)
			{
				this->Window.UseCursorMode(CursorMode::DISABLED);
				this->Window.UseCursorPos(cursorPos);
			}
			else
			{
				this->Window.UseCursorMode(CursorMode::NORMAL);
				this->Window.UseCursorPos({ this->Window.GetWidth() / 2, this->Window.GetHeight() / 2 });
			}
		}
	}

	if(this->Window.GetCursorMode() == CursorMode::NORMAL)
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		this->Console.SetSize({ this->Window.GetWidth() / 3.0f, this->Window.GetHeight() / 1.5f });
		this->Console.Draw("Debug Console");

		auto pos = this->GetRenderer().ViewPort.GetPosition();

		ImGui::SetNextWindowPos({ 0, Console.GetSize().y });
		ImGui::SetNextWindowSize({ Console.GetSize().x, 0.0f });
		ImGui::Begin("Fast Game Editor");

		ImGui::Checkbox("display mesh", &drawMesh);

		ImGui::Text("speed: %f", speed);
		ImGui::Text("position: (%f, %f, %f)", pos.x, pos.y, pos.z);

		ImGui::InputFloat("set speed", &speed);
		if (ImGui::InputFloat3("set position", &pos[0]))
		{
			this->GetRenderer().ViewPort.SetPosition(pos);
		}

		auto zoom = this->GetRenderer().ViewPort.GetZoom();

		ImGui::Text("zoom / fov: %f", zoom);
		if (ImGui::DragFloat("zoom", &zoom, 0.1f, 0.1f, 10.0f))
		{
			this->GetRenderer().ViewPort.SetZoom(zoom);
		}

		ImGui::End();
	}
	float deltaRot = 0.1f * TimeDelta;
	ArcObject.RotateY(deltaRot);

	this->GetRenderer().Clear();

	GridObject.Hide();
	this->DrawObjects(drawMesh);
	
	GridObject.Show();
	this->GetRenderer().Draw(GridObject);
}

void SandboxApp::OnDestroy()
{

}