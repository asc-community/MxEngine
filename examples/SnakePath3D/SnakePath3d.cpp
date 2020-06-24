#include "SnakePath3D.h"

void SnakePath3D::GenerateLevel()
{
	auto& camera = RenderManager::GetViewport();
	control->Reset();
	cellCount = (int)control->GenerateLevel(600);
	trace = stack<Cell*>();
	MxObject::GetByComponent(*camera).Transform->SetPosition(control->StartPosition - Vector3(0.5f, 0.0f, 0.5f));
}

void SnakePath3D::OnCreate()
{
	Random::SetSeed((unsigned int)Time::System());
	control = MakeUnique<GameController>(20, 1.0f, 0.05f, 1.0f);
	GenerateLevel();
	// add objects here

	auto lightObject = MxObject::Create();
	auto globalLight = lightObject->GetComponent<DirectionalLight>();
	
	globalLight->AmbientColor  = { 0.1f, 0.1f, 0.1f };
	globalLight->DiffuseColor  = { 0.5f, 0.5f, 0.5f };
	globalLight->SpecularColor = { 1.0f, 1.0f, 1.0f };
	
	auto cameraObject = MxObject::Create();
	auto controller = cameraObject->AddComponent<CameraController>();

	controller->SetMoveSpeed(5.0f);
	controller->SetRotateSpeed(0.75f);
	controller->ListenWindowResizeEvent();
	auto input = cameraObject->AddComponent<InputControl>();
	input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
	input->BindRotation();
}

void SnakePath3D::OnUpdate()
{
	auto& camera = RenderManager::GetViewport();
	auto& transform = MxObject::GetByComponent(*camera).Transform;
	static Vector3 lastPos = transform->GetPosition();
	auto pos = transform->GetPosition();
	transform->SetPosition({ pos.x, 1.5f, pos.z });
	Cell* cell = control->GetCollidedCell(pos + Vector3(0.5f));

	if (cell != nullptr)  
	{
		if (cell->GetState() == Cell::STATE::VOID)
		{
			transform->SetPosition(lastPos);
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
		transform->SetPosition(lastPos);
	}
	if (trace.size() >= cellCount)
		GenerateLevel();

	lastPos = transform->GetPosition();
}

void SnakePath3D::OnDestroy()
{

}

SnakePath3D::SnakePath3D()
{
	CreateContext();
}