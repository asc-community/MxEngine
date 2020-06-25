#pragma once

void InitCamera(MxObject& object)
{	
	object.Name = "Camera";

	auto controller = object.AddComponent<CameraController>();
	auto skybox = object.AddComponent<Skybox>();
	auto input = object.AddComponent<InputControl>();
	skybox->Texture = AssetManager::LoadCubeMap("textures/dawn.jpg"_id);

	controller->Camera.SetZFar(100000.0f);
	controller->Camera.SetAspectRatio(WindowManager::GetWidth(), WindowManager::GetHeight());

	controller->SetMoveSpeed(5.0f);
	controller->SetRotateSpeed(0.75f);
	controller->ListenWindowResizeEvent();
	input->BindMovement(KeyCode::W, KeyCode::A, KeyCode::S, KeyCode::D, KeyCode::SPACE, KeyCode::LEFT_SHIFT);
	input->BindRotation();

	object.Transform->Translate(MakeVector3(1.0f, 3.0f, 0.0f));
}