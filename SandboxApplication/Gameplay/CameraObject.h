#pragma once

void InitCamera(MxObject& object)
{	
	object.Name = "Camera";

	auto controller = object.AddComponent<CameraController>();
	auto skybox = object.AddComponent<Skybox>();
	skybox->Texture = AssetManager::LoadCubeMap("textures/dawn.jpg"_id);

	const auto& window = Application::Get()->GetWindow();
	controller->Camera.SetZFar(100000.0f);
	controller->Camera.SetAspectRatio((float)window.GetWidth(), (float)window.GetHeight());

	controller->SetMoveSpeed(5.0f);
	controller->SetRotateSpeed(0.75f);
	controller->ListenWindowResizeEvent();
	controller->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
	controller->BindRotation();

	object.Transform->Translate(MakeVector3(1.0f, 3.0f, 0.0f));
}