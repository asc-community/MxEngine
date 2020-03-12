#include "SnakePath3D.h"

void SnakePath3D::GenerateLevel()
{
	auto& camera = GetRenderer().ViewPort;
	control->Reset();
	cellCount = (int)control->GenerateLevel(600);
	trace = stack<Cell*>();
	camera.SetPosition(control->StartPosition - Vector3(0.5f, 0.0f, 0.5f));
}

void SnakePath3D::OnCreate()
{
	Random::SetSeed(Time::System());
	control = MakeUnique<GameController>(20, 1.0f, 0.05f, 1.0f);
	GenerateLevel();
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
	//controller.Translate(control->GetSize() / 2, 1.5f, control->GetSize() / 2);
	controller.SetMoveSpeed(5.0f);
	controller.SetRotateSpeed(0.75f);

	ConsoleBinding("Console").Bind(KeyCode::GRAVE_ACCENT);
	AppCloseBinding("AppClose").Bind(KeyCode::ESCAPE);
	InputBinding("CameraControl", Renderer.ViewPort)
		.BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT)
		.BindRotation();
}

void SnakePath3D::OnUpdate()
{
	auto& camera = this->GetRenderer().ViewPort;
	static Vector3 lastPos = camera.GetPosition();
	auto pos = camera.GetPosition();
	this->GetRenderer().ViewPort.SetPosition({ pos.x, 1.5f, pos.z });
	Cell* cell = control->GetCollidedCell(pos + Vector3(0.5f));

	if (cell != nullptr)  
	{
		if (cell->GetState() == Cell::STATE::VOID)
		{
			camera.SetPosition(lastPos);
		}
		else if (cell->GetState() == Cell::STATE::EMPTY)
		{
			if (trace.empty() || Cell::CommonSide(cell->Coord, trace.top()->Coord))
			{
				cell->SetState(Cell::STATE::LAST);
				if (!trace.empty()) trace.top()->SetState(Cell::STATE::MARKED);
				trace.emplace(cell);
			}
		}
		else if (cell->GetState() == Cell::STATE::MARKED && GetWindow().IsKeyHeld(KeyCode::SPACE))
		{
			while (!trace.empty() && trace.top() != cell)
			{
				trace.top()->SetState(Cell::STATE::EMPTY);
				trace.pop();
			}
			cell->SetState(Cell::STATE::LAST);
		}
	}
	else
	{
		camera.SetPosition(lastPos);
	}
	if (trace.size() >= cellCount)
		GenerateLevel();

	lastPos = camera.GetPosition();
}

void SnakePath3D::OnDestroy()
{

}

SnakePath3D::SnakePath3D()
{
	this->ResourcePath = "Resources/";
	this->CreateContext();
}